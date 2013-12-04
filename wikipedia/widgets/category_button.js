const Gdk = imports.gi.Gdk;
const GdkPixbuf = imports.gi.GdkPixbuf;
const GObject = imports.gi.GObject;
const Gtk = imports.gi.Gtk;
const Lang = imports.lang;

const Utils = imports.wikipedia.utils;

const CATEGORY_LABEL_LEFT_MARGIN = 25;  // pixels
const CATEGORY_LABEL_BOTTOM_MARGIN = 20;  // pixels
const CATEGORY_BUTTON_RIGHT_MARGIN = 20;  // pixels
const CATEGORY_BUTTON_BOTTOM_MARGIN = 20;  // pixels
// The following two are corrections because GTK 3.8 doesn't have baseline
// alignment. Remove and align properly in GTK 3.10. FIXME
const CATEGORY_LABEL_BASELINE_CORRECTION = 0; // pixels
const CATEGORY_BUTTON_BASELINE_CORRECTION = 10; // pixels
const _HOVER_ARROW_URI = '/com/endlessm/wikipedia-domain/assets/category_hover_arrow.png';
const MAIN_CATEGORY_SCREEN_WIDTH_PERCENTAGE = 0.37;

GObject.ParamFlags.READWRITE = GObject.ParamFlags.READABLE | GObject.ParamFlags.WRITABLE;

const CategoryButton = new Lang.Class({
    Name: 'CategoryButton',
    Extends: Gtk.Button,
    Properties: {
        // resource URI for the category's accompanying image
        'image-uri': GObject.ParamSpec.string('image-uri',
            'Image URI',
            'Resource URI for the image file accompanying the category',
            GObject.ParamFlags.READWRITE | GObject.ParamFlags.CONSTRUCT,
            ''),

        // Title of the category to display
        'category-title': GObject.ParamSpec.string('category-title',
            'Category title',
            'Display name for the category',
            GObject.ParamFlags.READWRITE | GObject.ParamFlags.CONSTRUCT,
            ''),

        // Boolean whether this button is clickable
        'clickable-category': GObject.ParamSpec.boolean('clickable-category',
            'Clickable Category',
            'Flag whether this category button should be clickable',
            GObject.ParamFlags.READWRITE | GObject.ParamFlags.CONSTRUCT_ONLY,
            true),   

        // Boolean whether this button is the main category
        'is-main-category': GObject.ParamSpec.boolean('is-main-category',
            'Is Main Category',
            'Flag whether this category button is the main category',
            GObject.ParamFlags.READWRITE | GObject.ParamFlags.CONSTRUCT_ONLY,
            false)        
    },

    _init: function(props) {
        // Get ready for property construction
        this._image_uri = null;
        this._category_title = null;
        this._clickable_category = null;
        this._is_main_category = null;
        this._pixbuf = null;

        this._grid = new Gtk.Grid({
            orientation: Gtk.Orientation.HORIZONTAL,
            expand: true,
            valign: Gtk.Align.END
        });
        this._label = new Gtk.Label({
            margin_left: CATEGORY_LABEL_LEFT_MARGIN,
            margin_bottom: CATEGORY_LABEL_BOTTOM_MARGIN - CATEGORY_LABEL_BASELINE_CORRECTION,
            hexpand: true,
            halign: Gtk.Align.START,
            xalign: 0.0,  // deprecated Gtk.Misc properties; necessary because
            wrap: true,   // "wrap" doesn't respect "halign"
            width_chars: 18,
            max_width_chars: 20
        });
        this._arrow = new Gtk.Image({
            resource: _HOVER_ARROW_URI,
            margin_right: CATEGORY_BUTTON_RIGHT_MARGIN,
            margin_bottom: CATEGORY_BUTTON_BOTTOM_MARGIN + CATEGORY_BUTTON_BASELINE_CORRECTION,
            halign: Gtk.Align.END,
            valign: Gtk.Align.END,
            opacity: 0
        });

        let context = this._label.get_style_context();
        context.add_class(EndlessWikipedia.STYLE_CLASS_TITLE);
        context.add_class(EndlessWikipedia.STYLE_CLASS_CATEGORY);
        context.add_class(EndlessWikipedia.STYLE_CLASS_FRONT_PAGE);

        // Parent constructor sets all properties
        this.parent(props);

        // Put widgets together
        this._grid.add(this._label);
        this._grid.add(this._arrow);
        this.add(this._grid);
        this.show_all();

        this.connect("enter", Lang.bind(this, function (w) { this._arrow.opacity = 1; }));
        this.connect("leave", Lang.bind(this, function (w) { this._arrow.opacity = 0; }));
    },

    get image_uri() {
        return this._image_uri;
    },

    set image_uri(value) {
        this._image_uri = value;
    },

    get category_title() {
        return this._category_title;
    },

    set category_title(value) {
        this._category_title = value;
        if(this._label)
            this._label.set_text(value.toUpperCase());
    },

    get clickable_category() {
        return this._clickable_category;
    },

    set clickable_category(value) {
        this._clickable_category = value;
        if(this._clickable_category) {
            this.get_style_context().add_class(EndlessWikipedia.STYLE_CLASS_CATEGORY_CLICKABLE);
        } else {
            this.get_style_context().remove_class(EndlessWikipedia.STYLE_CLASS_CATEGORY_CLICKABLE);
        }
    },

    get is_main_category() {
        return this._is_main_category;
    },

    set is_main_category(value) {
        this._is_main_category = value;
        if(this._is_main_category) {
            let context = this._label.get_style_context();
            context.add_class(EndlessWikipedia.STYLE_CLASS_MAIN);
            this._label.margin_bottom = 0;
            this._label.width_chars = 8;
            this._label.max_width_chars = 9;
        }
    },

    // OVERRIDES

    vfunc_get_preferred_width: function() {
        if(this._is_main_category) {
            let toplevel = this.get_toplevel();
            if(toplevel == null)
                return this.parent();
            let width = toplevel.get_allocated_width() * MAIN_CATEGORY_SCREEN_WIDTH_PERCENTAGE;
            return [width, width];
        } else {
            return this.parent();
        }
    },

    // Reloads the pixbuf from the gresource at the proper size if necessary
    _update_pixbuf: function () {
        if (this._image_uri === "" || this._image_uri === null)
            return;
        let allocation = this.get_allocation();
        if (this._pixbuf === null || this._pixbuf.get_width() !== allocation.width ||
                                     this._pixbuf.get_height() !== allocation.height)
            this._pixbuf = Utils.load_pixbuf_cover(Utils.resourceUriToPath(this._image_uri),
                                                   allocation.width, allocation.height);
    },

    vfunc_draw: function (cr) {
        this._update_pixbuf();
        if (this._pixbuf !== null) {
            Gdk.cairo_set_source_pixbuf(cr, this._pixbuf, 0, 0);
            cr.paint();
        }
        let ret = this.parent(cr);
        // We need to manually call dispose on cairo contexts. This is somewhat related to the bug listed here
        // https://bugzilla.gnome.org/show_bug.cgi?id=685513 for the shell. We should see if they come up with
        // a better fix in the future, i.e. fix this through gjs.
        cr.$dispose();
        return ret;
    }
});
