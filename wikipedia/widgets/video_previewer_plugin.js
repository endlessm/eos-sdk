const Lang = imports.lang;
const Gst = imports.gi.Gst;
const Gtk = imports.gi.Gtk;
const GtkClutter = imports.gi.GtkClutter;
const Clutter = imports.gi.Clutter;
const ClutterGst = imports.gi.ClutterGst;
const Endless = imports.gi.Endless;

const VideoPreviewerPlugin = {
    init: function () {
        GtkClutter.init(null);
        ClutterGst.init(null);
        // this._supported_types = Endless.get_clutter_gst_supported_formats();
    },

    supports_type: function (type) {
        // Hackzors
        return type.indexOf("video") != -1;
    },

    get_widget: function (file) {
        let embed = new GtkClutter.Embed();
        let stage = embed.get_stage();
        stage.set_layout_manager(new Clutter.BinLayout());
        let video_texture = new ClutterGst.VideoTexture({
            x_expand: true,
            y_expand: true
        });
        video_texture.set_uri(file.get_uri());
        video_texture.playing = true;
        stage.add_child(video_texture);
        return embed;
    }
};
