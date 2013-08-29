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
const CATEGORY_LABEL_BENTON_SANS_CORRECTION = 0; // pixels
const CATEGORY_BUTTON_BENTON_SANS_CORRECTION = 10; // pixels
const _HOVER_ARROW_URI = '/com/endlessm/wikipedia-domain/assets/category_hover_arrow.png';
const MAIN_CATEGORY_SCREEN_WIDTH_PERCENTAGE = 0.37;

GObject.ParamFlags.READWRITE = GObject.ParamFlags.READABLE | GObject.ParamFlags.WRITABLE;

const CategoryButton = new Lang.Class({
    Name: 'CategoryButton',
    Extends: Gtk.EventBox,
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
    Signals: {
        'clicked': {}
    },

    _init: function(props) {
        // Get ready for property construction
        this._image_uri = null;
        this._category_title = null;
        this._clickable_category = null;
        this._is_main_category = null;

        this._overlay = new Gtk.Overlay();
        this._eventbox = new Gtk.EventBox({
            expand: true
        });
        this._eventbox_grid = new Gtk.Grid({
            orientation: Gtk.Orientation.HORIZONTAL,
            hexpand: true,
            valign: Gtk.Align.END
        });
        this._label = new Gtk.Label({
            margin_left: CATEGORY_LABEL_LEFT_MARGIN,
            margin_bottom: CATEGORY_LABEL_BOTTOM_MARGIN - CATEGORY_LABEL_BENTON_SANS_CORRECTION,
            hexpand: true,
            halign: Gtk.Align.START,
            xalign: 0.0,  // deprecated Gtk.Misc properties; necessary because
            wrap: true    // "wrap" doesn't respect "halign"
        });
        this._arrow = new Gtk.Image({
            resource: _HOVER_ARROW_URI,
            margin_right: CATEGORY_BUTTON_RIGHT_MARGIN,
            margin_bottom: CATEGORY_BUTTON_BOTTOM_MARGIN + CATEGORY_BUTTON_BENTON_SANS_CORRECTION,
            halign: Gtk.Align.END,
            valign: Gtk.Align.END,
            opacity: 0
        });

        let context = this._label.get_style_context();
        context.add_class(EndlessWikipedia.STYLE_CLASS_TITLE);
        context.add_class(EndlessWikipedia.STYLE_CLASS_CATEGORY);
        context.add_class(EndlessWikipedia.STYLE_CLASS_FRONT_PAGE);
        this._image = new Gtk.Image({
            expand: true,
            halign: Gtk.Align.FILL,
            valign: Gtk.Align.FILL
        });

        // Parent constructor sets all properties
        this.parent(props);

        // Put widgets together
        this.add(this._overlay);
        this._overlay.add(this._image);
        this._eventbox_grid.add(this._label);
        this._eventbox_grid.add(this._arrow);
        this._eventbox.add(this._eventbox_grid);
        this._overlay.add_overlay(this._eventbox);
        this.show_all();

        // Connect signals
        this.connect('button-press-event',
            Lang.bind(this, this._onButtonPress));
    },

    get image_uri() {
        return this._image_uri;
    },

    set image_uri(value) {
        this._image_uri = value;
        if(this._image) {
            let allocation = this.get_allocation();
            let new_pixbuf = Utils.load_pixbuf_cover(Utils.resourceUriToPath(this._image_uri),
                allocation.width, allocation.height);
            this._image.set_from_pixbuf(new_pixbuf);
        }
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
            //Hover events/effects only trigger if the button is clickable.
            this._eventbox.add_events(Gdk.EventMask.ENTER_NOTIFY_MASK |
                Gdk.EventMask.LEAVE_NOTIFY_MASK);
            this._eventbox.connect('enter-notify-event',
                Lang.bind(this, function(widget, event) {
                    this._eventbox.set_state_flags(Gtk.StateFlags.PRELIGHT, false);
                    this._arrow.opacity = 1.0;
                }));
            this._eventbox.connect('leave-notify-event',
                Lang.bind(this, function(widget, event) {
                    this._eventbox.unset_state_flags(Gtk.StateFlags.PRELIGHT);
                    this._arrow.opacity = 0.0;
                }));
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

    vfunc_size_allocate: function(allocation) {
        this.parent(allocation);
        if(this._image_uri !== "" && this._image_uri != null) {
            let new_pixbuf = Utils.load_pixbuf_cover(Utils.resourceUriToPath(this._image_uri),
                allocation.width, allocation.height);
            this._image.set_from_pixbuf(new_pixbuf);
        }
    },

    // HANDLERS

    _onButtonPress: function(widget, event) {
        this.emit('clicked')
    }
});
