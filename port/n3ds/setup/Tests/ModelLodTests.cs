// New3DS integration additions by Kevin Boggs.
using HaloSetup;
using System.Threading;

// Kevin Boggs: ModelLodTests
static class ModelLodTests
{
    // Kevin Boggs: Run
    public static void Run()
    {
        const int side=24;byte[] vertices=new byte[side*side*32];var indices=new List<ushort>();
        for(int y=0;y<side;y++)for(int x=0;x<side;x++) {
            int offset=(y*side+x)*32;
            Array.Copy(BitConverter.GetBytes((float)x),0,vertices,offset,4);
            Array.Copy(BitConverter.GetBytes((float)y),0,vertices,offset+4,4);
            Array.Copy(BitConverter.GetBytes((short)(x*100)),0,vertices,offset+24,2);
            Array.Copy(BitConverter.GetBytes((short)(y*100)),0,vertices,offset+26,2);
            vertices[offset+29]=253;vertices[offset+30]=255;vertices[offset+31]=127;
            if(x<side-1 && y<side-1) {
                int a=y*side+x;foreach(int i in new[]{a,a+1,a+side,a+1,a+side+1,a+side})indices.Add((ushort)i);
            }
        }
        ushort[][] lod=ModelLods.Simplify(vertices,indices.ToArray());
        Data.Require(lod[0].Length<side*side*3/4 && lod[1].Length<indices.Count*3/4,"Planar mesh did not simplify.");
        var retained=new HashSet<ushort>(lod[0]);
        for(int i=0;i<side;i++)foreach(int edge in new[]{i,i*side,i*side+side-1,(side-1)*side+i})
            Data.Require(retained.Contains((ushort)edge),"An open boundary moved.");
        for(int i=0;i<lod[1].Length;i+=3) {
            int a=lod[0][lod[1][i]],b=lod[0][lod[1][i+1]],c=lod[0][lod[1][i+2]];
            Data.Require((b%side-a%side)*(c/side-a/side)-(b/side-a/side)*(c%side-a%side)>0,"Triangle flipped or collapsed.");
        }
        // A duplicated-position UV seam must keep both authored vertices.
        byte[] seam=new byte[vertices.Length+32];Array.Copy(vertices,seam,vertices.Length);
        Array.Copy(vertices,(side+1)*32,seam,vertices.Length,32);seam[vertices.Length+24]^=127;
        var seamIndices=indices.ToArray();seamIndices[0]=(ushort)(side*side);
        ushort[][] seamLod=ModelLods.Simplify(seam,seamIndices);
        Data.Require(seamLod[0].Contains((ushort)(side+1)) && seamLod[0].Contains((ushort)(side*side)),"UV seam removed.");
        using(var cancel=new CancellationTokenSource()) {
            cancel.Cancel();bool cancelled=false;
            try{ModelLods.Simplify(vertices,indices.ToArray(),cancel.Token);}catch(OperationCanceledException){cancelled=true;}
            Data.Require(cancelled,"Simplification ignored cancellation.");
        }
        bool rejected=false;try{ModelLods.Simplify(vertices,new ushort[]{0,1,65535});}catch(SetupException){rejected=true;}
        Data.Require(rejected,"Invalid mesh index accepted.");
        Console.WriteLine($"PASS model LOD topology, winding, boundary/UV seams, cancellation: {side*side}->{lod[0].Length} vertices, {indices.Count/3}->{lod[1].Length/3} triangles");
    }
}
