const GObject = imports.gi.GObject;
const Gtk = imports.gi.Gtk;
const Lang = imports.lang;

const CATEGORY_DESCRIPTION_WIDTH = 520;

const FixedSizeTextView = new Lang.Class({
    Name: "FixedSizeTextView",
    Extends: Gtk.TextView,

    vfunc_get_preferred_width:function(){
        return [CATEGORY_DESCRIPTION_WIDTH, CATEGORY_DESCRIPTION_WIDTH];
    }
});
