const EndlessWikipedia = imports.wikipedia.EndlessWikipedia;
const GObject = imports.gi.GObject;
const GdkPixbuf = imports.gi.GdkPixbuf;
const Gtk = imports.gi.Gtk;
const Lang = imports.lang;

const BoxWithBg = imports.wikipedia.widgets.BoxWithBg;
const ScaledImage = imports.wikipedia.widgets.scaled_image;

const CATEGORY_DESCRIPTION_WIDTH = 520;
const SUBMENU_SEPARATOR_A_URI = "/com/endlessm/wikipedia-domain/assets/submenu_separator_shadow_a.png";
const SPLASH_SEPARATOR_URI = "/com/endlessm/wikipedia-domain/assets/category_splash_separator_shadow.png";
const INTRO_TITLE_SEPARATOR_URI = "/com/endlessm/wikipedia-domain/assets/introduction_title_separator.png";

GObject.ParamFlags.READWRITE = GObject.ParamFlags.READABLE | GObject.ParamFlags.WRITABLE;

function _resourceUriToPath(uri) {
    if(uri.startsWith('resource://'))
        return uri.slice('resource://'.length);
    throw new Error('Resource URI did not start with "resource://"');
}

const PrebuiltCategoryPage = new Lang.Class({
    Name: 'PrebuiltCategoryPage',
    Extends: Gtk.Frame,
    Properties: {
        'title': GObject.ParamSpec.string('title',
            'Name of category',
            'Name of the category to be displayed at the top of the category page',
            GObject.ParamFlags.READWRITE | GObject.ParamFlags.CONSTRUCT,
            ''),
        'description': GObject.ParamSpec.string('description',
            'Description',
            'Description of the category (excerpt from Wiki text)',
            GObject.ParamFlags.READWRITE | GObject.ParamFlags.CONSTRUCT,
            ''),
        // resource URI for the category's accompanying image
        'image-uri': GObject.ParamSpec.string('image-uri',
            'Image URI',
            'Resource URI for the image file accompanying the category',
            GObject.ParamFlags.READWRITE | GObject.ParamFlags.CONSTRUCT,
            '')
    },

    _init: function(props) {
        this._vbox = new BoxWithBg.BoxWithBg({
            name: "category_info",
            orientation: Gtk.Orientation.VERTICAL,
            expand:true
        });
        this._vbox.set_size_request(CATEGORY_DESCRIPTION_WIDTH, -1);

        this._title = null;
        this._description = null;

        this._frame = new Gtk.Frame({
            name: "category_frame",
            expand: true,
        });

        this._layout_grid = new Gtk.Grid({
            orientation: Gtk.Orientation.HORIZONTAL,
            expand: true,
            halign: Gtk.Align.END
        });

        this._title_label = new Gtk.Label({
            name:"category_title",
            expand: false,
            halign: Gtk.Align.START
        });

        this._submenu_separator = new ScaledImage.ScaledImage({
            resource: SUBMENU_SEPARATOR_A_URI,
            constraint: Gtk.Orientation.VERTICAL,
            halign: Gtk.Align.END
        });

        this._splash_separator = new ScaledImage.ScaledImage({
            resource: SPLASH_SEPARATOR_URI,
            constraint: Gtk.Orientation.VERTICAL,
            halign: Gtk.Align.END
        });

        this._description_separator = new ScaledImage.ScaledImage({
            resource: INTRO_TITLE_SEPARATOR_URI,
            constraint: Gtk.Orientation.HORIZONTAL
        });

        this._description_text_view = new Gtk.TextView({
            name:"category_description",
            expand: true,
            margin_left: 6, // stupid Benton Sans correction
            sensitive: false,
            editable: false,
            cursor_visible: false
        });

        this._description_text_view.set_pixels_inside_wrap(10);
        this._description_text_view.set_wrap_mode(Gtk.WrapMode.WORD);
        this._description_text_view.right_margin = 20;

        this._description_scrolled_window = new Gtk.ScrolledWindow({
            name: 'category_scrolled_window',
            halign: Gtk.Align.FILL
        });

        this._description_scrolled_window.add(this._description_text_view);
        this._description_scrolled_window.set_policy(Gtk.PolicyType.NEVER,
            Gtk.PolicyType.AUTOMATIC);

        this._inner_grid = new Gtk.Grid({
            orientation: Gtk.Orientation.VERTICAL,
            expand: true,
            margin_left: 45,
            margin_right: 45,
            margin_bottom: 15
        });

        this.parent(props);

        this._inner_grid.add(this._title_label);
        this._inner_grid.add(this._description_separator);
        this._inner_grid.add(this._description_scrolled_window);
        this._vbox.add(this._inner_grid);

        this._layout_grid.add(this._splash_separator);
        this._layout_grid.add(this._vbox);
        
        this._overlay = new Gtk.Overlay({halign:Gtk.Align.END});
        this._overlay.add(this._layout_grid);
        this._overlay.add_overlay(this._submenu_separator);

        this._frame.add(this._overlay);
        this.add(this._frame);
        this._category_provider = new Gtk.CssProvider();

    },

    get title() {
        return this._title;
    },

    set title(value) {
        this._title = value;
        if(this._title_label)
            this._title_label.label = value.toUpperCase();
    },

    get description() {
        return this._description;
    },

    set description(value) {
        this._description = value;
        if(this._description_text_view)
            this._description_text_view.buffer.set_text(value, -1);
    },

    get image_uri(){
        return this._image_uri;
    },

    set image_uri(value){
        this._image_uri = value;
        let frame_css = "#category_frame{background-image: url('" + value + "');background-repeat:no-repeat;background-size:cover;}";
        this._category_provider.load_from_data(frame_css);
        let context = this._frame.get_style_context();
        context.add_provider(this._category_provider, Gtk.STYLE_PROVIDER_PRIORITY_APPLICATION);
    }
});