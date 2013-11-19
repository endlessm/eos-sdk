const GObject = imports.gi.GObject;
const Gtk = imports.gi.Gtk;
const Lang = imports.lang;

const BoxWithBg = new Lang.Class({
    Name: "BoxWithBg",
    Extends: Gtk.Box,

    vfunc_draw: function(cr) {
        let width = this.get_allocated_width();
        let height = this.get_allocated_height();
        let context = this.get_style_context();
        Gtk.render_background(context, cr, 0, 0, width, height);
        Gtk.render_frame(context, cr, 0, 0, width, height);

        let ret = this.parent(cr);
        cr.$dispose();
        return ret;
    }
});

