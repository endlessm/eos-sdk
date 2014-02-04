const Gdk = imports.gi.Gdk;
const GdkPixbuf = imports.gi.GdkPixbuf;
const GObject = imports.gi.GObject;
const Gtk = imports.gi.Gtk;
const Lang = imports.lang;

const CompositeButton = imports.wikipedia.widgets.composite_button;
const Utils = imports.wikipedia.utils;

const CATEGORY_LABEL_LEFT_MARGIN_PIXELS = 5;  // in addition to the 20px below
const CATEGORY_LABEL_SPACING_PIXELS = 20;
const CATEGORY_BUTTON_SIZE_PIXELS = 42;
const CATEGORY_BUTTON_RESOURCE_URI = 'resource:///com/endlessm/wikipedia-domain/assets/wikipedia-category-forward-symbolic.svg';
const CATEGORY_MIN_WIDTH = 120; // pixels

GObject.ParamFlags.READWRITE = GObject.ParamFlags.READABLE | GObject.ParamFlags.WRITABLE;

const CategoryButton = new Lang.Class({
    Name: 'CategoryButton',
    Extends: CompositeButton.CompositeButton,
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

        this._inner_grid = new Gtk.Grid({
            valign: Gtk.Align.END,
            halign: Gtk.Align.FILL,
            border_width: CATEGORY_LABEL_SPACING_PIXELS,
            column_spacing: CATEGORY_LABEL_SPACING_PIXELS,
            expand: true
        });
        this._label = new Gtk.Label({
            margin_left: CATEGORY_LABEL_LEFT_MARGIN_PIXELS,
            halign: Gtk.Align.START,
            valign: Gtk.Align.BASELINE,
            xalign: 0.0,  // deprecated Gtk.Misc properties; necessary because
            wrap: true,   // "wrap" doesn't respect "halign"
            max_width_chars: 20
        });
        this._arrow = new Gtk.Image({
            gicon: new Gio.FileIcon({
                file: Gio.File.new_for_uri(CATEGORY_BUTTON_RESOURCE_URI)
            }),
            pixel_size: CATEGORY_BUTTON_SIZE_PIXELS,
            hexpand: true,
            halign: Gtk.Align.END,
            valign: Gtk.Align.END
        });
        this._arrow.get_style_context().add_class(Gtk.STYLE_CLASS_IMAGE);

        let context = this._label.get_style_context();
        context.add_class(EndlessWikipedia.STYLE_CLASS_TITLE);
        context.add_class(EndlessWikipedia.STYLE_CLASS_CATEGORY);
        context.add_class(EndlessWikipedia.STYLE_CLASS_FRONT_PAGE);

        // Parent constructor sets all properties
        this.parent(props);

        // Put widgets together
        this.setSensitiveChildren([this._arrow]);
        this._inner_grid.add(this._label);
        this._inner_grid.add(this._arrow);
        this.add(this._inner_grid);
        this.show_all();

        // For some reason, on the NUC, setting opacity in CSS for this button does not work.
        // So we have to set it in Gtk code. Also, we have to set the opacity
        // to zero upfront. I am putting that here instead of in the initialisation
        // of the arrow since it is part of this NUC-specific hack 
        this._arrow.connect('state-flags-changed', Lang.bind(this, this._update_appearance));
        this._arrow.set_opacity(0)
    },

    _update_appearance: function(widget) {
        // If button is hovered over and/or pressed, then show the arrow icon
        if ((widget.get_state_flags() & Gtk.StateFlags.ACTIVE ||
            widget.get_state_flags() & Gtk.StateFlags.PRELIGHT) &&
            this._clickable_category) {
            this._arrow.set_opacity(1);
            return false;
        }
        // If no hover or press, then hide the arrow icon
        this._arrow.set_opacity(0);
        return false;
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
        this._update_appearance(this._arrow);
    },

    get is_main_category() {
        return this._is_main_category;
    },

    set is_main_category(value) {
        this._is_main_category = value;
        if(this._is_main_category) {
            let context = this._label.get_style_context();
            context.add_class(EndlessWikipedia.STYLE_CLASS_MAIN);
            this._label.max_width_chars = 9;
        }
    },

    // OVERRIDES

    // Sometimes our label content runs too long and the min window width can
    // be greater than the screen width. So we provide our own min width for
    // category buttons here, and allow the GtkLabels to be cut off if there's
    // no space. We ask for width for height management so the height will be
    // allocated first.
    vfunc_get_request_mode: function() {
        return Gtk.SizeRequestMode.WIDTH_FOR_HEIGHT;
    },

    vfunc_get_preferred_width_for_height: function(height) {
        let natural_width = this.parent(height)[1];
        return [CATEGORY_MIN_WIDTH, natural_width];
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
