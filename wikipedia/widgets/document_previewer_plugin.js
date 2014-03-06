const Lang = imports.lang;
const Gtk = imports.gi.Gtk;
const Endless = imports.gi.Endless;
const EvinceView = imports.gi.EvinceView;
const EvinceDocument = imports.gi.EvinceDocument;

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
        let view = new EvinceView.View();
        view.set_model(document_model);
        let scrolled_window = new Gtk.ScrolledWindow({
            expand: true
        });
        scrolled_window.add(view);
        return scrolled_window;
    }
};
