const Lang = imports.lang;
const Gtk = imports.gi.Gtk;
const Endless = imports.gi.Endless;
const EvinceView = imports.gi.EvinceView;
const EvinceDocument = imports.gi.EvinceDocument;

const Toolbar = new Lang.Class({
    Name: 'Toolbar',
    Extends: Gtk.Frame,

    _init: function (params) {
        this.parent(params);
        this.get_style_context().add_class("previewer-toolbar");
        this._grid = new Gtk.Grid({
            orientation: Gtk.Orientation.HORIZONTAL
        });
        this.add(this._grid);
    },

    add_tool: function (widget) {
        this._grid.add(widget);
        widget.connect("pressed", Lang.bind(this, function () {
            this.get_style_context().add_class("faded-out");
        }));
    }
});


const DocumentPreviewerPlugin = {
    init: function () {
        EvinceDocument.init();
        this._supported_types = Endless.get_evince_supported_formats();
    },

    supports_type: function (type) {
        return this._supported_types.indexOf(type) != -1;
    },

    get_widget: function (file) {
        let document = EvinceDocument.Document.factory_get_document(file.get_uri());
        let document_model = new EvinceView.DocumentModel(document);
        document_model.set_document(document);
        document_model.set_sizing_mode(EvinceView.SizingMode.FREE);
        let view = new EvinceView.View();
        view.set_model(document_model);
        let scrolled_window = new Gtk.ScrolledWindow({
            expand: true
        });
        scrolled_window.add(view);

        let zoom_in_button = Gtk.Button.new_from_icon_name ("zoom-in",
                                                            Gtk.IconSize.LARGE_TOOLBAR);
        zoom_in_button.connect("pressed", function (widget) {
            view.zoom_in();
        });
        let zoom_out_button = Gtk.Button.new_from_icon_name ("zoom-out",
                                                             Gtk.IconSize.LARGE_TOOLBAR);
        zoom_out_button.connect("pressed", function (widget) {
            view.zoom_out();
        });

        let toolbar = new Toolbar({
            halign: Gtk.Align.CENTER,
            valign: Gtk.Align.END
        });
        toolbar.add_tool(zoom_out_button);
        toolbar.add_tool(zoom_in_button);

        let overlay = new Gtk.Overlay();
        overlay.add(scrolled_window);
        overlay.add_overlay(toolbar);
        return overlay;
    }
};
