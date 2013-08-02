const Format = imports.format;
const GdkPixbuf = imports.gi.GdkPixbuf;
const GObject = imports.gi.GObject;
const Gtk = imports.gi.Gtk;
const Lang = imports.lang;

String.prototype.format = Format.format;
GObject.ParamFlags.READWRITE = GObject.ParamFlags.READABLE | GObject.ParamFlags.WRITABLE;

const BACKGROUND_CSS_TEMPLATE = "\
Gjs_ScaledImage {\n\
    background-image: url('resource://%s');\n\
    background-size: %s;\n\
    background-position: %s %s;\n\
    background-repeat: no-repeat;\n\
}";

const ScaledImage = new Lang.Class({
    Name: 'ScaledImage',
    Extends: Gtk.EventBox,
    Properties: {
        'constraint': GObject.ParamSpec.enum('constraint',
            'Constraint direction',
            'Orientation in which the size of the image should be constrained',
            GObject.ParamFlags.READWRITE | GObject.ParamFlags.CONSTRUCT,
            Gtk.Orientation, Gtk.Orientation.HORIZONTAL),
        'resource': GObject.ParamSpec.string('resource',
            'Resource path',
            'Resource path for the image',
            GObject.ParamFlags.READWRITE | GObject.ParamFlags.CONSTRUCT,
            '')
    },

    _init: function(props) {
        this._constraint = null;
        this._resource_path = null;
        this._pixbuf = null;
        this._css_provider = null;
        this.parent(props);
    },

    // OVERRIDES

    vfunc_get_request_mode: function() {
        if(this._constraint == Gtk.Orientation.HORIZONTAL)
            return Gtk.SizeRequestMode.HEIGHT_FOR_WIDTH;
        return Gtk.SizeRequestMode.WIDTH_FOR_HEIGHT;
    },

    vfunc_get_preferred_width_for_height: function(height) {
        if(!this._pixbuf)
            return this.parent(height);
        let source_width = this._pixbuf.width;
        let source_height = this._pixbuf.height;
        let width = (height / source_height) * source_width;
        return [width, width];
    },

    vfunc_get_preferred_height_for_width: function(width) {
        if(!this._pixbuf)
            return this.parent(width);
        let source_width = this._pixbuf.width;
        let source_height = this._pixbuf.height;
        let height = (width / source_width) * source_height;
        return [height, height];
    },

    vfunc_size_allocate: function(allocation) {
        if(this._constraint == Gtk.Orientation.VERTICAL
            && this.valign != Gtk.Align.FILL) {
            printerr("ScaledImage Warning: Setting constraint to VERTICAL and\
 valign to anything but FILL makes no sense");
            this.valign = Gtk.Align.FILL;
        }
        if(this._constraint == Gtk.Orientation.HORIZONTAL
            && this.halign != Gtk.Align.FILL) {
            printerr("ScaledImage Warning: Setting constraint to HORIZONTAL and\
 halign to anything but FILL makes no sense");
            this.halign = Gtk.Align.FILL;
        }
        this.parent(allocation);
    },

    // PROPERTIES

    get constraint() {
        return this._constraint;
    },

    set constraint(value) {
        this._constraint = value;
    },

    get resource() {
        return this._resource_path;
    },

    set resource(value) {
        this._resource_path = value;
        this._pixbuf = GdkPixbuf.Pixbuf.new_from_resource(this._resource_path);
        this._updateImage();
    },

    // PRIVATE

    _gtk_align_to_css_align: function(align, orientation) {
        switch(align) {
        case Gtk.Align.START:
            if(orientation == Gtk.Orientation.VERTICAL)
                return "top";
            return "left";
        case Gtk.Align.END:
            if(orientation == Gtk.Orientation.VERTICAL)
                return "bottom";
            return "right";
        }
        return "center";
    },

    _updateImage: function() {
        if(this._resource_path === null)
            return;

        let context = this.get_style_context();

        if(this._css_provider !== null)
            context.remove_provider(this._css_provider);

        let scaling;
        if(this._constraint == Gtk.Orientation.HORIZONTAL)
            scaling = "100% auto";
        else
            scaling = "auto 100%";

        let css = BACKGROUND_CSS_TEMPLATE.format(this._resource_path, scaling,
            this._gtk_align_to_css_align(this.valign, Gtk.Orientation.VERTICAL),
            this._gtk_align_to_css_align(this.halign,
                Gtk.Orientation.HORIZONTAL));
        this._css_provider = new Gtk.CssProvider();
        this._css_provider.load_from_data(css);
        context.add_provider(this._css_provider,
            Gtk.STYLE_PROVIDER_PRIORITY_USER);
    }
});

// const Gio = imports.gi.Gio;
// Gtk.init(null);
// let resource = Gio.Resource.load('data/endless_brazil.gresource');
// resource._register();
// let w = new Gtk.Window();
// let i = new ScaledImage({
//     resource: '/com/endlessm/brazil/category_images/cuisine.jpg',
//     constraint: Gtk.Orientation.HORIZONTAL,
//     valign: Gtk.Align.END,
// });
// w.add(i);
// w.connect('destroy', Gtk.main_quit);
// w.show_all();
// Gtk.main();
