const GdkPixbuf = imports.gi.GdkPixbuf;
const GObject = imports.gi.GObject;
const Gtk = imports.gi.Gtk;
const Lang = imports.lang;

const Utils = imports.utils;

const TITLE_LABEL_SCREEN_WIDTH_PERCENTAGE = 0.37;
const TITLE_LABEL_LEFT_MARGIN = 20;  // pixels
const TITLE_LABEL_BOTTOM_MARGIN = 20;  // pixels
const TITLE_LABEL_BENTON_SANS_CORRECTION = 20; // pixels

const TitleLabelView = new Lang.Class({
    Name: 'TitleLabelView',
    Extends: Gtk.Overlay,
    Properties: {
        'title': GObject.ParamSpec.string('title',
            'Front page title',
            'Name of the Wikipedia-based application',
            GObject.ParamFlags.READWRITE | GObject.ParamFlags.CONSTRUCT,
            ''),
        'image-uri': GObject.ParamSpec.string('image-uri',
            'Image URI',
            'Image URI for title image',
            GObject.ParamFlags.READWRITE | GObject.ParamFlags.CONSTRUCT,
            '')
    },

    _init: function(props) {
        this._title = null;
        this._image_uri = null;
        this._label = new Gtk.Label({
            halign: Gtk.Align.START,
            valign: Gtk.Align.END,
            margin_left: TITLE_LABEL_LEFT_MARGIN,
            margin_bottom: TITLE_LABEL_BOTTOM_MARGIN - TITLE_LABEL_BENTON_SANS_CORRECTION
        });
        this._image = new Gtk.Image();

        let context = this._label.get_style_context()
        context.add_class(EndlessWikipedia.STYLE_CLASS_TITLE);

        this.parent(props);

        this.add(this._image);
        this.add_overlay(this._label);
    },

    // OVERRIDES

    // Ensure that this widget is 37% of the window's width
    vfunc_get_preferred_width: function() {
        let toplevel = this.get_toplevel();
        if(toplevel == null)
            return this.parent();
        let width = toplevel.get_allocated_width() * TITLE_LABEL_SCREEN_WIDTH_PERCENTAGE;
        return [width, width];
    },

    vfunc_size_allocate: function(allocation) {
        this.parent(allocation);
        if(this._image_uri !== "" && this._image_uri != null) {
            this._updateImage(Utils.resourceUriToPath(this._image_uri),
                allocation.width, allocation.height);
        }
    },

    // PROPERTIES

    get title() {
        return this._title;
    },

    set title(value) {
        this._title = value;
        if(this._label)
            this._label.label = value.toUpperCase();
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

    // PRIVATE

    _updateImage: function(res_path, dest_width, dest_height) {
        let [source_width, source_height] = [dest_width, dest_height];
        // TODO: We need to get the size of the source image, so right now we
        // are loading the image twice, once to get the size, and the again at
        // the proper size. We should eventually use a GdkPixbuf.Loader and
        // connect to the size-prepared signal, as described in the
        // documentation
        let temp_pixbuf = GdkPixbuf.Pixbuf.new_from_resource(res_path);
        let source_aspect = temp_pixbuf.width / temp_pixbuf.height;
        let dest_aspect = dest_width / dest_height;
        if(dest_aspect > source_aspect)
            source_height = -1;
        else
            source_width = -1;
        let source_pixbuf = GdkPixbuf.Pixbuf.new_from_resource_at_scale(res_path,
            source_width, source_height, true);
        let cropped_pixbuf = source_pixbuf;
        if(dest_width < source_pixbuf.width || dest_height < source_pixbuf.height)
            cropped_pixbuf = source_pixbuf.new_subpixbuf(0, 0, dest_width, dest_height);
        this._image.set_from_pixbuf(cropped_pixbuf);
    }
});
