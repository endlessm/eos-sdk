const Lang = imports.lang;
const Gtk = imports.gi.Gtk;

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
    Extends: Gtk.Bin,

    _init: function (params) {
        this.parent(params);

        this._preview_widget = null;
    },

    preview_file: function (file) {
        if (this._preview_widget !== null) {
            this.remove(this._preview_widget);
            this._preview_widget.close();
        }

        let type = file.query_info("standard::content-type", Gio.FileQueryInfoFlags.NONE, null).get_content_type();
        if (!type) {
            print("Could not recognize type");
            return;
        }

        for (let preview_plugin of PreviewPlugins) {
            if (preview_plugin.supports_type(type)) {
                this._preview_widget = preview_plugin.get_widget(file);
                this.add(this._preview_widget);
                this.show_all();
                return;
            }
        }
        print("No supported previewer found");
    },

    close_file: function() {
    }
});
