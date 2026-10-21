/* The FractumSeraph/OpenCE fork's overlay (port/linux/src/ui_overlay.h) on
 * the 3DS: what its ChupathingyCE edition's screens over the menus draw
 * with (Online Games' server browser, browser_screen.c; the invite prompt;
 * moderation), which on the desktops is drawn by OpenGL at the window's
 * resolution (ui_overlay.c, d3d8_gl.c). Here it is drawn as the menus are,
 * in their 640x480, during their own pass (render_ui_widgets, which
 * Halo3DS's n3ds_menu_draw_screen calls), in the order it is asked:
 * shapes as the game's screen quads (draw_quad: the game's own white
 * texture, alpha blended, as a cinematic's fade), a gradient as bands of
 * them; text in the menus' own fonts (ui\large_ui, ui\small_ui) scaled to
 * the size asked (rasterizer_text_set_scale). Rounded corners are square;
 * pictures, platform logos and cutouts are not drawn (the browser shows
 * its own cards for the maps). */
#include "cseries.h"
#include "rasterizer/rasterizer.h"
#include "tag_files/tag_groups.h"
#include "text/draw_string.h"
#include "text/font_group.h"

#include <string.h>

unsigned int n3ds_engine_tag_generation(void); /* (engine_cache.c: a new one each time tags load or unload) */

void draw_quad(rectangle2d *rectangle, pixel32 color);

enum
{
	OVERLAY_GRADIENT_BANDS = 12,
	OVERLAY_TEXT_LENGTH = 256,
};

/* (ui_overlay.h's fonts, buttons and alignments) */
enum
{
	UI_FONT_REGULAR,
	UI_FONT_BOLD,
	UI_FONT_MENU,
	UI_FONT_MENU_SMALL,
	UI_FONT_TITLE,
};
enum
{
	UI_ALIGN_LEFT,
	UI_ALIGN_CENTER,
	UI_ALIGN_RIGHT,
};
enum
{
	UI_BUTTON_A,
	UI_BUTTON_B,
	UI_BUTTON_X,
	UI_BUTTON_Y,
	UI_BUTTON_START,
	UI_BUTTON_LEFT_TRIGGER,
	UI_BUTTON_RIGHT_TRIGGER,
	UI_BUTTON_LEFT_SHOULDER,
	UI_BUTTON_RIGHT_SHOULDER,
	UI_BUTTON_DPAD_LEFT,
	UI_BUTTON_DPAD_RIGHT,
	UI_BUTTON_BACK,
	NUMBER_OF_UI_BUTTONS
};

/* ---------- colors and shapes */

/* 0xRRGGBBAA to the game's 0xAARRGGBB */
static pixel32 overlay_pixel(unsigned int color)
{
	return (pixel32)(((color & 0xFF) << 24) | (color >> 8));
}

static unsigned int overlay_mix(unsigned int a, unsigned int b, float t)
{
	unsigned int result = 0;
	int shift;

	for (shift = 0; shift < 32; shift += 8)
	{
		float x = (float)((a >> shift) & 0xFF);
		float y = (float)((b >> shift) & 0xFF);
		unsigned int c = (unsigned int)(x + (y - x) * t + 0.5f);

		result |= (c > 255 ? 255 : c) << shift;
	}
	return result;
}

static unsigned int overlay_alpha(unsigned int color, float scale)
{
	unsigned int alpha = (unsigned int)((float)(color & 0xFF) * scale + 0.5f);

	return (color & 0xFFFFFF00) | (alpha > 255 ? 255 : alpha);
}

static void overlay_quad(float x, float y, float width, float height, unsigned int color)
{
	rectangle2d bounds;

	if (width <= 0 || height <= 0 || !(color & 0xFF))
		return;
	bounds.x0 = (short)(x + 0.5f);
	bounds.y0 = (short)(y + 0.5f);
	bounds.x1 = (short)(x + width + 0.5f);
	bounds.y1 = (short)(y + height + 0.5f);
	if (bounds.x1 <= bounds.x0)
		bounds.x1 = bounds.x0 + 1;
	if (bounds.y1 <= bounds.y0)
		bounds.y1 = bounds.y0 + 1;
	draw_quad(&bounds, overlay_pixel(color));
}

void ui_overlay_rect(float x, float y, float width, float height, float radius, unsigned int color)
{
	(void)radius;
	overlay_quad(x, y, width, height, color);
}

void ui_overlay_gradient(float x, float y, float width, float height, float radius, unsigned int top,
	unsigned int bottom)
{
	int band;

	(void)radius;
	if (top == bottom)
	{
		overlay_quad(x, y, width, height, top);
		return;
	}
	{
		/* (a band each 32 units high, 2 to 12 of them: each is a draw of
		its own) */
		int bands = (int)(height / 32);

		if (bands < 2)
			bands = 2;
		if (bands > OVERLAY_GRADIENT_BANDS)
			bands = OVERLAY_GRADIENT_BANDS;
		for (band = 0; band < bands; band++)
		{
			float y0 = y + height * band / bands;
			float y1 = y + height * (band + 1) / bands;

			overlay_quad(x, y0, width, y1 - y0, overlay_mix(top, bottom, (band + 0.5f) / bands));
		}
	}
}

void ui_overlay_outline(float x, float y, float width, float height, float radius, float thickness,
	unsigned int color)
{
	(void)radius;
	if (thickness < 1)
		thickness = 1;
	overlay_quad(x, y, width, thickness, color);
	overlay_quad(x, y + height - thickness, width, thickness, color);
	overlay_quad(x, y + thickness, thickness, height - 2 * thickness, color);
	overlay_quad(x + width - thickness, y + thickness, thickness, height - 2 * thickness, color);
}

/* the Xbox menus' pieces, in their blues */
void ui_overlay_panel(float x, float y, float width, float height, int lit)
{
	ui_overlay_gradient(x, y, width, height, 0, lit ? 0x16386AE0 : 0x0C2346D8, lit ? 0x0B2850E0 : 0x07183AD8);
	ui_overlay_outline(x, y, width, height, 0, lit ? 2.0f : 1.0f, lit ? 0x8FD0FFFF : 0x3F7FC8E0);
}

void ui_overlay_band(float left, float right, float top, float bottom)
{
	overlay_quad(left, top, right - left, 2, 0x6FB8FFF0);
	ui_overlay_gradient(left, top + 2, right - left, bottom - top - 2, 0, 0x1A4C8AA0, 0x0A2A5A60);
}

void ui_overlay_prompt_bar(float left, float right, float top, float bottom)
{
	overlay_quad(left, top, right - left, 1, 0x4F90D8E0);
	overlay_quad(left, top + 1, right - left, bottom - top - 1, 0x050F22D0);
}

void ui_overlay_layer(int layer)
{
	/* (drawn as asked: a later layer's shapes go over an earlier one's) */
	(void)layer;
}

void ui_overlay_cutout(float x, float y, float width, float height)
{
	(void)x;
	(void)y;
	(void)width;
	(void)height;
}

void ui_overlay_picture(int serial, const unsigned char *pixels, int width, int height, float x, float y,
	float draw_width, float draw_height, unsigned int color)
{
	(void)serial;
	(void)pixels;
	(void)width;
	(void)height;
	(void)x;
	(void)y;
	(void)draw_width;
	(void)draw_height;
	(void)color;
}

int ui_overlay_platform(int platform, float size, float x, float y, unsigned int color, int shadow)
{
	(void)platform;
	(void)size;
	(void)x;
	(void)y;
	(void)color;
	(void)shadow;
	return 0;
}

int ui_overlay_available(void)
{
	return 1;
}

void ui_overlay_present(int x, int y, int width, int height, int window_width, int window_height)
{
	(void)x;
	(void)y;
	(void)width;
	(void)height;
	(void)window_width;
	(void)window_height;
}

/* ---------- text */

/* UTF-8 to the game's 16-bit characters (beyond the first plane: '?') */
static void overlay_text_wide(const char *text, wchar_t *wide, int size)
{
	const unsigned char *in = (const unsigned char *)(text ? text : "");
	int length = 0;

	while (*in && length < size - 1)
	{
		unsigned long code = *in++;
		int more = 0;

		if (code >= 0xF0)
		{
			code &= 0x07;
			more = 3;
		}
		else if (code >= 0xE0)
		{
			code &= 0x0F;
			more = 2;
		}
		else if (code >= 0xC0)
		{
			code &= 0x1F;
			more = 1;
		}
		else if (code >= 0x80)
		{
			code = '?';
		}
		while (more-- > 0 && (*in & 0xC0) == 0x80)
			code = (code << 6) | (*in++ & 0x3F);
		wide[length++] = (wchar_t)(code > 0xFFFF ? '?' : code);
	}
	wide[length] = 0;
}

/* the menus' font for a size, and the scale it is drawn at to be that
   high (the large font's capitals are about 13 of the 640x480's units
   high, the small one's about 10: menu_tags.c's measures) */
/* a font's tag, looked up once for the tags loaded (tag_loaded walks every
   tag, and a screen draws a hundred strings a frame): looked up again when
   another map's tags are (n3ds_engine_tag_generation) */
static long overlay_font_tag(int large)
{
	static const char *const names[2] = { "ui\\small_ui", "ui\\large_ui" };
	static unsigned int generations[2];
	static int known[2];
	static long indexes[2];
	unsigned int generation = n3ds_engine_tag_generation();

	if (!known[large] || generations[large] != generation)
	{
		indexes[large] = tag_loaded(FONT_GROUP_TAG, names[large]);
		generations[large] = generation;
		known[large] = 1;
	}
	return indexes[large];
}

static long overlay_font(int font, float size, real *scale)
{
	boolean large = font == UI_FONT_TITLE || size >= 15.0f;
	long index = overlay_font_tag(large ? 1 : 0);

	*scale = size / (large ? 18.0f : 14.0f);
	if (*scale < 0.6f)
		*scale = 0.6f;
	if (*scale > 2.5f)
		*scale = 2.5f;
	return index;
}

static float overlay_text_measure(long font_index, real scale, wchar_t const *wide)
{
	rectangle2d bounds = { 0, 0, 480, 2000 };
	rectangle2d text_bounds = { 0 };
	real_argb_color white = { 1.0f, 1.0f, 1.0f, 1.0f };

	if (font_index == NONE || !wide[0])
		return 0;
	draw_string_set_draw_mode(font_index, NONE, 0, 0, &white);
	draw_string_set_indents(0, 0);
	draw_unicode_string_compute_bounds(&bounds, wide, &text_bounds, NULL);
	return (float)(text_bounds.x1 - text_bounds.x0) * scale;
}

float ui_overlay_text_width(int font, float size, const char *text)
{
	wchar_t wide[OVERLAY_TEXT_LENGTH];
	real scale;
	long font_index = overlay_font(font, size, &scale);

	overlay_text_wide(text, wide, OVERLAY_TEXT_LENGTH);
	return overlay_text_measure(font_index, scale, wide);
}

static float overlay_text_draw(int font, float size, float x, float y, int align, unsigned int color,
	const char *text, float offset, float alpha)
{
	wchar_t wide[OVERLAY_TEXT_LENGTH];
	real scale;
	long font_index = overlay_font(font, size, &scale);
	float width;
	float left;
	rectangle2d bounds;
	real_argb_color argb;

	overlay_text_wide(text, wide, OVERLAY_TEXT_LENGTH);
	if (font_index == NONE || !wide[0])
		return 0;
	width = overlay_text_measure(font_index, scale, wide);
	left = align == UI_ALIGN_CENTER ? x - width / 2 : align == UI_ALIGN_RIGHT ? x - width : x;
	left += offset;
	y += offset;
	argb.alpha = (float)(color & 0xFF) / 255.0f * alpha;
	argb.red = (float)((color >> 24) & 0xFF) / 255.0f;
	argb.green = (float)((color >> 16) & 0xFF) / 255.0f;
	argb.blue = (float)((color >> 8) & 0xFF) / 255.0f;
	/* (the text laid out at its own size from the top left, then scaled
	about that corner to the size asked) */
	bounds.x0 = (short)left;
	bounds.y0 = (short)y;
	bounds.x1 = (short)(left + width / scale + 4);
	bounds.y1 = (short)(y + size / scale * 2 + 4);
	draw_string_set_draw_mode(font_index, NONE, 0, 0, &argb);
	draw_string_set_indents(0, 0);
	rasterizer_text_set_scale(scale, left, y);
	rasterizer_draw_unicode_string(&bounds, NULL, NULL, 0, wide);
	rasterizer_text_set_scale(1.0f, 0, 0);
	return width;
}

float ui_overlay_text(int font, float size, float x, float y, int align, unsigned int color, const char *text)
{
	return overlay_text_draw(font, size, x, y, align, color, text, 0, 1.0f);
}

float ui_overlay_text_glow(int font, float size, float x, float y, int align, unsigned int color,
	unsigned int glow, const char *text)
{
	/* (the text alone: a glow or shadow would draw it twice, and the 3DS's
	text buffer is a frame's budget, engine_gpu_text.c) */
	(void)glow;
	return overlay_text_draw(font, size, x, y, align, color, text, 0, 1.0f);
}

/* ---------- buttons */

/* a button as the 3DS's own: its letter on a badge (A, B, X, Y in the
   Xbox's colors, as the menus' prompts show them) */
static const char *const overlay_button_labels[NUMBER_OF_UI_BUTTONS] =
	{ "A", "B", "X", "Y", "START", "ZL", "ZR", "L", "R", "<", ">", "SELECT" };
static const unsigned int overlay_button_colors[NUMBER_OF_UI_BUTTONS] =
	{ 0x3FA83CFF, 0xC8302CFF, 0x2C64C8FF, 0xD8B02CFF, 0x4A4F5AFF, 0x4A4F5AFF, 0x4A4F5AFF, 0x4A4F5AFF,
	  0x4A4F5AFF, 0x4A4F5AFF, 0x4A4F5AFF, 0x4A4F5AFF };

float ui_overlay_button_width(int button, float size)
{
	float text;

	if (button < 0 || button >= NUMBER_OF_UI_BUTTONS)
		return 0;
	text = ui_overlay_text_width(UI_FONT_BOLD, size * 0.8f, overlay_button_labels[button]);
	return text + size * 0.5f > size ? text + size * 0.5f : size;
}

float ui_overlay_button(int button, float size, float x, float y, unsigned int color)
{
	float width = ui_overlay_button_width(button, size);
	float dim = (float)(color & 0xFF) / 255.0f;

	if (!width)
		return 0;
	overlay_quad(x, y, width, size, overlay_alpha(overlay_button_colors[button], dim));
	overlay_text_draw(UI_FONT_BOLD, size * 0.8f, x + width / 2, y + size * 0.1f, UI_ALIGN_CENTER,
		overlay_alpha(0xFFFFFFFF, dim), overlay_button_labels[button], 0, 1.0f);
	return width;
}
