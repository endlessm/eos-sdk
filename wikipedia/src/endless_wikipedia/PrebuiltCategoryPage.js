const GObject = imports.gi.GObject;
const GdkPixbuf = imports.gi.GdkPixbuf;
const Gtk = imports.gi.Gtk;
const Lang = imports.lang;

const EndlessWikipedia = imports.endless_wikipedia.EndlessWikipedia;
//const BoxWithBg = imports.endless_wikipedia.BoxWithBg;

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


        this._vbox = new EndlessWikipedia.BoxWithBg({
            name: "category_info",
            orientation: Gtk.Orientation.VERTICAL
        });


        this._title = null;
        this._description = null;

        this._frame = new Gtk.Frame({
            name: "category_frame",
            expand: true,
        });

        this._grid = new Gtk.Grid({
            orientation: Gtk.Orientation.VERTICAL,
            expand: true,
            halign: Gtk.Align.END
        });
        this._title_label = new Gtk.Label({
            name:"category_title",
            vexpand: false,
            halign: Gtk.Align.START,
            margin_left:45,
            margin_right:45
        });
        this._separator = new Gtk.Separator({
            orientation: Gtk.Orientation.HORIZONTAL,
            vexpand: false
        });


        this._separator = new Gtk.Image({
            file: "../data/category_images/introduction_title_separator.png"
        });

        this._description_label = new Gtk.Label({
            name:"category_description",
            valign: Gtk.Align.START,
            halign: Gtk.Align.START,
            margin_left:45,
            margin_right:45
        });
        this._description_label.set_line_wrap(true);
        this._description_label.set_max_width_chars(40);
        this._image = new Gtk.Image({
            //resource: uri
            expand: true
        });
        this._grid.set_size_request(100, 100);


        this.parent(props);

        this._vbox.pack_start(this._title_label, false, false, 0);
        this._vbox.pack_start(this._separator, false, false, 0);
        this._vbox.pack_start(this._description_label, true, true, 0);
        this._grid.add(this._vbox);
        //this.add(this._grid);
        this.add(this._frame);
        this._frame.add(this._grid);
        let uri = _resourceUriToPath("resource:///com/endlessm/brazil/category_images/cuisine.jpg");
        let frame_css = "#category_frame{background-image: url('../data/category_images/cuisine.jpg');background-repeat:no-repeat;background-size:cover;}";
        let provider = new Gtk.CssProvider();

       
        provider.load_from_data(frame_css, frame_css.length, null);

        let context = this._frame.get_style_context();
        context.add_provider(provider, Gtk.STYLE_PROVIDER_PRIORITY_APPLICATION);

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
        if(this._description_label)
            this._description_label.label = value;
    },

    get image_uri(){
        return this._image_uri;
    },

    set image_uri(value){
        this._image_uri = value;
        let allocation = this.get_allocation();
        //this._updateImage(res_path, allocation.width, allocation.height);
        let width = 1000;
        let height = 1000;
        //print(this._grid.get_width());
    }
});