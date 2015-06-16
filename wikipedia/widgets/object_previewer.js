const Lang = imports.lang;
const Gtk = imports.gi.Gtk;
const GObject = imports.gi.GObject;

const Utils = imports.wikipedia.widgets.utils;

// previewer plugins
const ImagePreviewerPlugin = imports.wikipedia.widgets.image_previewer_plugin;
const VideoPreviewerPlugin = imports.wikipedia.widgets.video_previewer_plugin;
const DocumentPreviewerPlugin = imports.wikipedia.widgets.document_previewer_plugin;
const PreviewPlugins = [ImagePreviewerPlugin.ImagePreviewerPlugin,
                        VideoPreviewerPlugin.VideoPreviewerPlugin,
                        DocumentPreviewerPlugin.DocumentPreviewerPlugin];

function init_plugins() {
    for (let preview_plugin of PreviewPlugins) {
        preview_plugin.init();
    }
}

const ObjectPreviewer = new Lang.Class({
    Name: 'ObjectPreviewer',
    Extends: Gtk.Frame,

    _init: function (params) {
        params = params || {};
        params["margin"] = 100;
        this.parent(params);

        this._preview_widget = null;
    },

    preview_file: function (file) {
        this.close_preview();

        let type = file.query_info("standard::content-type", Gio.FileQueryInfoFlags.NONE, null).get_content_type();
        if (!type) {
            printerr("Could not recognize type");
            return;
        }

        for (let preview_plugin of PreviewPlugins) {
            if (preview_plugin.supports_type(type)) {
                this._preview_widget = preview_plugin.get_widget(file);
                this._preview_widget_bindings = Utils.bind_align_expand(this._preview_widget, this);
                this.add(this._preview_widget);
                this.show_all();
                return;
            }
        }
        printerr("No supported previewer found");
    },

    close_preview: function() {
        if (this._preview_widget !== null) {
            this._preview_widget_bindings.map(function (binding) { binding.unbind(); });
            this._preview_widget_bindings = [];
            this.remove(this._preview_widget);
            this._preview_widget = null;
        }
    }
});
