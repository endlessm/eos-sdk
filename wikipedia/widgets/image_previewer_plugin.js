const Lang = imports.lang;
const GObject = imports.gi.GObject;
const Gtk = imports.gi.Gtk;
const Gdk = imports.gi.Gdk;
const GdkPixbuf = imports.gi.GdkPixbuf;
const GLib = imports.gi.GLib;

const CSS_OVERRIDE_TEMPLATE = "#image-frame { background-image: url('%filename'); background-size: 100% 100%; }";

const ImagePreviewerPlugin = {
    init: function () {
        // nothing needed, stock gtk
    },

    supports_type: function (type) {
        if (!this._supported_types) {
            let formats = GdkPixbuf.Pixbuf.get_formats();
            this._supported_types = formats.reduce(function(type_list, format) {
                return type_list.concat(format.get_mime_types());
            }, []);
        }
        return this._supported_types.indexOf(type) != -1;
    },

    get_widget: function (file) {
        let override = CSS_OVERRIDE_TEMPLATE.replace("%filename", file.get_uri());
        let provider = new Gtk.CssProvider();
        provider.load_from_data(override);
        Gtk.StyleContext.add_provider_for_screen(Gdk.Screen.get_default(),
                                                 provider,
                                                 Gtk.STYLE_PROVIDER_PRIORITY_APPLICATION)
        let frame = new Gtk.Frame({
            expand: true,
            name: "image-frame"
        });
        return frame;
    }
};
