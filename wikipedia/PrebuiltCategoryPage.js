const Endless = imports.gi.Endless;
const GObject = imports.gi.GObject;
const Gtk = imports.gi.Gtk;
const Lang = imports.lang;

const CategoryBackButton = imports.wikipedia.widgets.category_back_button;
const FixedSizeTextView = imports.wikipedia.widgets.FixedSizeTextView;

GObject.ParamFlags.READWRITE = GObject.ParamFlags.READABLE | GObject.ParamFlags.WRITABLE;

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

    Signals: {
        'go-back-home':{}
    },

    _get_info_box: function(){
        this._title_label = new Gtk.Label({
            name: 'category-title',
            expand: false,
            halign: Gtk.Align.START,
            xalign: 0.0, // deprecated Gtk.Misc property; necessary because
            wrap: true,  // "wrap" doesn't respect "halign"
            width_chars: 15,
            max_width_chars: 18
        });

        let description_separator = new Gtk.Frame();
        description_separator.get_style_context().add_class(
            EndlessWikipedia.STYLE_CLASS_CATEGORY_HORIZONTAL_SEPARATOR);

        this._description_text_view = new FixedSizeTextView.FixedSizeTextView({
            name: 'category-description',
            sensitive: false,
            editable: false,
            cursor_visible: false,
            pixels_inside_wrap: 10,
            wrap_mode: Gtk.WrapMode.WORD
        });

        let description_scrolled_window = new Gtk.ScrolledWindow({
            name: 'category-scrolled-window',
            halign: Gtk.Align.FILL,
            vexpand: true,
            hscrollbar_policy: Gtk.PolicyType.NEVER,
            vscrollbar_policy: Gtk.PolicyType.AUTOMATIC
        });
        description_scrolled_window.add(this._description_text_view);

        let vertical_separator = new Gtk.Frame();
        vertical_separator.get_style_context().add_class(
            EndlessWikipedia.STYLE_CLASS_CATEGORY_VERTICAL_SEPARATOR);

        let grid = new Gtk.Grid();
        grid.attach(this._title_label, 0, 0, 1, 1);
        grid.attach(description_separator, 0, 1, 1, 1);
        grid.attach(description_scrolled_window, 0, 2, 1, 1);
        grid.attach(vertical_separator, 1, 0, 1, 3);

        let frame = new Gtk.Frame({
            name: 'category-info',
            vexpand: true
        });
        frame.add(grid);
        return frame;
    },

    _init: function(props) {
        this._title = null;
        this._description = null;
        this._category_provider = new Gtk.CssProvider();
        this.parent(props);

        let info_box = this._get_info_box();

        let back_button = new CategoryBackButton.CategoryBackButton({
            name: 'category-back-button',
            expand: true,
            halign: Gtk.Align.START,
            valign: Gtk.Align.FILL
        });
        back_button.connect('clicked', Lang.bind(this, function() {
            this.emit('go-back-home');
        }));
        
        let vertical_separator = new Gtk.Frame();
        vertical_separator.get_style_context().add_class(
            EndlessWikipedia.STYLE_CLASS_CATEGORY_VERTICAL_SEPARATOR);

        let grid = new Gtk.Grid({
            orientation: Gtk.Orientation.HORIZONTAL
        });
        grid.add(back_button);
        grid.add(vertical_separator);
        grid.add(info_box);
        this.add(grid);

        // Add style contexts for CSS
        let context = this.get_style_context();
        context.add_class(EndlessWikipedia.STYLE_CLASS_CATEGORY_PAGE);
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
        if (value) {
            let frame_css = "#category_frame{background-image: url('" + value + "');background-repeat:no-repeat;background-size:cover;}";
            this._category_provider.load_from_data(frame_css);
            let context = this.get_style_context();
            context.add_provider(this._category_provider, Gtk.STYLE_PROVIDER_PRIORITY_APPLICATION);
        }
    }
});
