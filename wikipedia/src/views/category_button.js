const GdkPixbuf = imports.gi.GdkPixbuf;
const GObject = imports.gi.GObject;
const Gtk = imports.gi.Gtk;
const Lang = imports.lang;


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
            '')
    },
    Signals: {
        'clicked': {}
    },

    _init: function(props) {
        // Get ready for property construction
        this._image_uri = null;
        this._category_title = null;

        this._overlay = new Gtk.Overlay();
        this._eventbox = new Gtk.EventBox({
            vexpand: false,
            valign: Gtk.Align.END,
            halign: Gtk.Align.FILL
        })
        this._label = new Gtk.Label({
            halign: Gtk.Align.START
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
        this._eventbox.add(this._label);
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
            let res_path = Utils.resourceUriToPath(value);
            let allocation = this.get_allocation();
            this._updateImage(res_path, allocation.width, allocation.height);
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

    // OVERRIDES

    vfunc_size_allocate: function(allocation) {
        this.parent(allocation);
        this._updateImage(Utils.resourceUriToPath(this._image_uri),
            allocation.width, allocation.height);
    },

    // HANDLERS

    _onButtonPress: function(widget, event) {
        this.emit('clicked')
    },

    // PRIVATE

    _updateImage: function(res_path, width, height) {
        let [source_width, source_height] = [width, height];
        if(width > height)
            source_height = -1;
        else
            source_width = -1;
        let source_pixbuf = GdkPixbuf.Pixbuf.new_from_resource_at_scale(res_path,
            source_width, source_height, true);
        let cropped_pixbuf = source_pixbuf;
        if(width < source_pixbuf.width || height < source_pixbuf.height)
            cropped_pixbuf = source_pixbuf.new_subpixbuf(0, 0, width, height);
        this._image.set_from_pixbuf(cropped_pixbuf);
    }
});
