const Endless = imports.gi.Endless;
const Gettext = imports.gettext;
const GLib = imports.gi.GLib;
const GObject = imports.gi.GObject;
const Gtk = imports.gi.Gtk;
const Lang = imports.lang;

const BoxWithBg = imports.wikipedia.widgets.BoxWithBg;
const CompositeButton = imports.wikipedia.widgets.composite_button;
const Config = imports.wikipedia.config;
const FixedSizeTextView = imports.wikipedia.widgets.FixedSizeTextView;
const ScaledImage = imports.wikipedia.widgets.scaled_image;

const SHADOW_SEPARATOR_RESOURCE_PATH = "/com/endlessm/wikipedia-domain/assets/submenu_separator_shadow_a.png";
const INTRO_TITLE_SEPARATOR_URI = "/com/endlessm/wikipedia-domain/assets/introduction_title_separator.png";
const CATEGORY_BACK_BUTTON_ASSET_RESOURCE_URI = 'resource:///com/endlessm/wikipedia-domain/assets/wikipedia-category-back-symbolic.svg';
const CATEGORY_BACK_BUTTON_SIZE_PIXELS = 68;
const LEFT_MARGIN_FOR_TEXT = 45;

GObject.ParamFlags.READWRITE = GObject.ParamFlags.READABLE | GObject.ParamFlags.WRITABLE;

const _ = function(string) { return GLib.dgettext('eos-sdk', string); };
Gettext.bindtextdomain('eos-sdk', Config.DATADIR + '/locale');

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

    Signals: {
        'go-back-home':{}
    },

    _init: function(props) {
        this._shaded_box = new BoxWithBg.BoxWithBg({
            name: "category_info",
            orientation: Gtk.Orientation.VERTICAL,
            vexpand: true
        });

        this._title = null;
        this._description = null;

        this._layout_grid = new Gtk.Grid({
            orientation: Gtk.Orientation.HORIZONTAL,
            hexpand: true,
            halign: Gtk.Align.END
        });

        this._title_label = new Gtk.Label({
            name:"category_title",
            expand: false,
            halign: Gtk.Align.START,
            margin_left: LEFT_MARGIN_FOR_TEXT,
            xalign: 0.0, // deprecated Gtk.Misc property; necessary because
            wrap: true,  // "wrap" doesn't respect "halign"
            width_chars: 15,
            max_width_chars: 18
        });

        this._submenu_separator = new ScaledImage.ScaledImage({
            resource: SHADOW_SEPARATOR_RESOURCE_PATH,
            constraint: Gtk.Orientation.VERTICAL,
            halign: Gtk.Align.END
        });

        this._splash_separator = new ScaledImage.ScaledImage({
            resource: SHADOW_SEPARATOR_RESOURCE_PATH,
            constraint: Gtk.Orientation.VERTICAL,
            halign: Gtk.Align.END
        });

        this._description_separator = new ScaledImage.ScaledImage({
            resource: INTRO_TITLE_SEPARATOR_URI,
            constraint: Gtk.Orientation.HORIZONTAL
        });

        this._description_text_view = new FixedSizeTextView.FixedSizeTextView({
            name:"category_description",
            sensitive: false,
            editable: false,
            cursor_visible: false
        });

        this._description_text_view.set_pixels_inside_wrap(10);
        this._description_text_view.set_wrap_mode(Gtk.WrapMode.WORD);
        this._description_text_view.right_margin = 20;

        this._description_scrolled_window = new Gtk.ScrolledWindow({
            name: 'category_scrolled_window',
            halign: Gtk.Align.FILL,
            vexpand: true,
            margin_left: LEFT_MARGIN_FOR_TEXT
        });

        this._description_scrolled_window.add(this._description_text_view);

        this._description_scrolled_window.set_policy(Gtk.PolicyType.NEVER,
            Gtk.PolicyType.AUTOMATIC);

        let gicon = new Gio.FileIcon({
            file: Gio.File.new_for_uri(CATEGORY_BACK_BUTTON_ASSET_RESOURCE_URI)
        });
        let backButtonIcon = Gtk.Image.new_from_gicon(gicon,
            Gtk.IconSize.DIALOG);
        backButtonIcon.pixel_size = CATEGORY_BACK_BUTTON_SIZE_PIXELS;
        let backButtonLabel = new Gtk.Label({
            label: _("OTHER CATEGORIES")
        });
        let backButtonInnerGrid = new Gtk.Grid();
        this._back_button = new CompositeButton.CompositeButton({
            name: "category-back-button",
            expand: true,
            halign: Gtk.Align.START,
            valign: Gtk.Align.CENTER
        });
        backButtonInnerGrid.add(backButtonIcon);
        backButtonInnerGrid.add(backButtonLabel);
        this._back_button.add(backButtonInnerGrid);
        this._back_button.setSensitiveChildren([
            backButtonIcon,
            backButtonLabel
        ]);

        this._back_button.connect('clicked', Lang.bind(this, function() {
            this.emit('go-back-home');
        }));

        this.parent(props);

        this._shaded_box.add(this._title_label);
        this._shaded_box.add(this._description_separator);
        this._shaded_box.add(this._description_scrolled_window);

        this._layout_grid.add(this._splash_separator);
        this._layout_grid.add(this._shaded_box);
        
        this._overlay = new Gtk.Overlay({
            halign: Gtk.Align.END
        });
        this._overlay.add(this._layout_grid);
        this._overlay.add_overlay(this._submenu_separator);

        this._outer_most_grid = new Gtk.Grid({
            orientation: Gtk.Orientation.HORIZONTAL
        });
        this._outer_most_grid.add(this._back_button);
        this._outer_most_grid.add(this._overlay);

        this.add(this._outer_most_grid);
        this._category_provider = new Gtk.CssProvider();

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
        this._image_uri = value;
        let frame_css = "#category_frame{background-image: url('" + value + "');background-repeat:no-repeat;background-size:cover;}";
        this._category_provider.load_from_data(frame_css);
        let context = this.get_style_context();
        context.add_provider(this._category_provider, Gtk.STYLE_PROVIDER_PRIORITY_APPLICATION);
    }
});
