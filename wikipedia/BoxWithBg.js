const GObject = imports.gi.GObject;
const Gtk = imports.gi.Gtk;
const Lang = imports.lang;

const BoxWithBg = new Lang.Class({
    Name: "BoxWithBg",
    Extends: Gtk.Box,

    vfunc_draw: function(cairoContext) {
        let width = this.get_allocated_width();
        let height = this.get_allocated_height();
        let context = this.get_style_context();
        Gtk.render_background(context, cairoContext, 0, 0, width, height);
        Gtk.render_frame(context, cairoContext, 0, 0, width, height);

        return this.parent(cairoContext);
    }
});

