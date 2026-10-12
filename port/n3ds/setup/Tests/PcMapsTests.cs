// New3DS integration additions by Kevin Boggs.
using System;
using System.IO;
using System.Text;
using System.Collections.Generic;
using HaloSetup;
#if LEGACY
using Token = HaloSetup.CancellationToken;
using TokenSource = HaloSetup.CancellationTokenSource;
using Cancelled = HaloSetup.OperationCanceledException;
#else
using Token = System.Threading.CancellationToken;
using TokenSource = System.Threading.CancellationTokenSource;
using Cancelled = System.OperationCanceledException;
#endif

// Kevin Boggs: PcMapsTests - identical import and recovery tests on modern and legacy runtimes.
static class PcMapsTests
{
    // Kevin Boggs: Check
    static void Check(bool condition, string message)
    { if (!condition) throw new Exception(message); Console.WriteLine("PASS PC " + message); }

    // Kevin Boggs: Reject
    static void Reject(Action action, string message)
    {
        try { action(); } catch (SetupException) { Console.WriteLine("PASS PC " + message); return; }
        throw new Exception("Expected rejection: " + message);
    }

    // Kevin Boggs: Fixture
    static byte[] Fixture(string name)
    {
        byte[] bytes = new byte[4096];
        using (var stream = new MemoryStream(bytes))
        using (var writer = new BinaryWriter(stream))
        {
            if (name == "bitmaps" || name == "sounds" || name == "loc")
            {
                writer.Write(name == "bitmaps" ? 1 : name == "sounds" ? 2 : 3);
                writer.Write(2048); writer.Write(3072); writer.Write(1);
                stream.Position = 3072; writer.Write(0); writer.Write(8); writer.Write(16);
            }
            else
            {
                writer.Write(Encoding.ASCII.GetBytes("daeh")); writer.Write(7); writer.Write(bytes.Length);
                stream.Position = 16; writer.Write(2048); writer.Write(2048);
                stream.Position = 32; writer.Write(Encoding.ASCII.GetBytes(name));
                stream.Position = 96; writer.Write((short)(name == "ui" ? 2 : 1));
                stream.Position = 2044; writer.Write(Encoding.ASCII.GetBytes("toof"));
            }
        }
        return bytes;
    }

    // Kevin Boggs: Run
    public static void Run(string parent)
    {
        string root = Path.Combine(parent, "pc-tests"), source = Path.Combine(root, "source"), destination = Path.Combine(root, "card");
        string maps = Path.Combine(source, "maps");
        Directory.CreateDirectory(maps); Directory.CreateDirectory(destination);
        Action<string> report = delegate { };
        Check(PcMaps.Inspect(null, destination, new Token(), report).Count == 0, "optional folder stays optional");
        foreach (string name in new[] { "bloodgulch", "ui", "bitmaps", "sounds", "a10" })
            File.WriteAllBytes(Path.Combine(maps, name + ".map"), Fixture(name));
        var selected = PcMaps.Inspect(source, destination, new Token(), report);
        Check(selected.Count == 4, "game root accepts maps child and skips campaign files");
        string ui = Path.Combine(maps, "ui.map"), blood = Path.Combine(maps, "bloodgulch.map");
        foreach (byte version in new byte[] { 5, 97 })
        {
            byte[] bad = Fixture("bloodgulch"); bad[4] = version; File.WriteAllBytes(blood, bad);
            Reject(delegate { PcMaps.Inspect(maps, destination, new Token(), report); }, "unsupported map version");
        }
        File.WriteAllBytes(blood, Fixture("sidewinder"));
        Reject(delegate { PcMaps.Inspect(maps, destination, new Token(), report); }, "wrong map identity");
        byte[] invalid = Fixture("bloodgulch"); invalid[16] = 255; invalid[17] = 255; File.WriteAllBytes(blood, invalid);
        Reject(delegate { PcMaps.Inspect(maps, destination, new Token(), report); }, "out of bounds tag data");
        File.WriteAllBytes(blood, new byte[12]);
        Reject(delegate { PcMaps.Inspect(maps, destination, new Token(), report); }, "truncated map");
        File.WriteAllBytes(blood, Fixture("bloodgulch"));
        File.Delete(ui);
        Reject(delegate { PcMaps.Inspect(maps, destination, new Token(), report); }, "missing shared file");
        File.WriteAllBytes(ui, Fixture("ui"));
        string sounds = Path.Combine(maps, "sounds.map");
        File.WriteAllBytes(sounds, Fixture("bitmaps"));
        Reject(delegate { PcMaps.Inspect(maps, destination, new Token(), report); }, "wrong shared resource type");
        File.WriteAllBytes(sounds, Fixture("sounds"));
        Check(Directory.GetFileSystemEntries(destination).Length == 0, "failed preflight never writes destination");

        string game = Path.Combine(destination, "halo-source"); Directory.CreateDirectory(game);
        string xbox = Path.Combine(game, "ui.map"), save = Path.Combine(game, "save.bin");
        File.WriteAllText(xbox, "existing Xbox"); File.WriteAllText(save, "keep save");
        File.WriteAllText(Path.Combine(game, Hud.Name), "existing HUD");
        string launcher = Path.Combine(root, "Halo3DS.3dsx"); byte[] executable = new byte[64];
        Array.Copy(Encoding.ASCII.GetBytes("3DSX"), executable, 4); File.WriteAllBytes(launcher, executable);
        // Exercise the actual installer, not only the file-copy helper.
        var result = Installer.Run(new SetupOptions("", destination, launcher, null, true, source));
        string installed = Path.Combine(game, "pc-retail");
        Check(result.PcFiles == 4 && Data.HashFile(Path.Combine(installed, "bloodgulch.map")) == Data.HashFile(blood), "add PC maps through real installer");
        Check(File.ReadAllText(xbox) == "existing Xbox" && File.ReadAllText(save) == "keep save", "Xbox files and saves preserved");
        var timestamps = new Dictionary<string, DateTime>();
        foreach (string path in Directory.GetFiles(installed)) timestamps.Add(path, File.GetLastWriteTimeUtc(path));
        Installer.Run(new SetupOptions("", destination, launcher, null, true, maps));
        foreach (var pair in timestamps) Check(pair.Value == File.GetLastWriteTimeUtc(pair.Key), "repeat skips " + Path.GetFileName(pair.Key));
        Reject(delegate { PcMaps.Inspect(installed, destination, new Token(), report); }, "source inside destination rejected");
        string installedBlood = Path.Combine(installed, "bloodgulch.map"); File.WriteAllText(installedBlood, "different existing PC data");
        Reject(delegate { Installer.Run(new SetupOptions("", destination, launcher, null, true, maps)); }, "conflicting installed data rejected");
        Check(File.ReadAllText(installedBlood) == "different existing PC data", "conflict preserves existing file");

        string cancelled = Path.Combine(root, "cancelled");
        using (var cancel = new TokenSource())
        {
            try { PcMaps.Install(selected, cancelled, cancel.Token, delegate { cancel.Cancel(); }); throw new Exception("Cancellation ignored"); }
            catch (Cancelled) { Check(Directory.GetFiles(cancelled, "*", SearchOption.AllDirectories).Length == 0, "cancel removes partial copies"); }
        }
        string changed = Path.Combine(root, "changed");
        byte[] changedBytes = Fixture("bloodgulch"); changedBytes[3000] = 1; File.WriteAllBytes(blood, changedBytes);
        Reject(delegate { PcMaps.Install(selected, changed, new Token(), report); }, "source change detected during copy");
        Check(Directory.GetFiles(changed, "*", SearchOption.AllDirectories).Length == 0, "failed verification never publishes file");
        File.WriteAllBytes(blood, Fixture("bloodgulch")); File.WriteAllBytes(Path.Combine(maps, "loc.map"), Fixture("loc"));
        Check(PcMaps.Inspect(maps, changed, new Token(), report).Count == 5, "optional localization file included");
        string empty = Path.Combine(root, "empty"); Directory.CreateDirectory(empty);
        Reject(delegate { Installer.Run(new SetupOptions("", empty, launcher, null, true, maps)); }, "PC-only import requires Xbox base");
    }
}
