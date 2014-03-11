const Lang = imports.lang;
const Gst = imports.gi.Gst;
const Gtk = imports.gi.Gtk;
const GtkClutter = imports.gi.GtkClutter;
const Clutter = imports.gi.Clutter;
const ClutterGst = imports.gi.ClutterGst;
const Endless = imports.gi.Endless;

const ScalableGstPlayer = Lang.Class({
    Name: 'ScalableGstPlayer',
    Extends: GtkClutter.Embed,

    _init: function(params) {
        this._natural_width = 0;
        this._aspect = 1;

        params = params || {};
        params["halign"] = Gtk.Align.CENTER;
        params["valign"] = Gtk.Align.CENTER;
        this.parent(params);

        let stage = this.get_stage();
        stage.set_layout_manager(new Clutter.BinLayout());
        this._video_texture = new ClutterGst.VideoTexture({
            x_expand: true,
            y_expand: true
        });
        this._video_texture.connect('size-change', Lang.bind(this, this._texture_size_changed));
        stage.add_child(this._video_texture);
    },

    vfunc_get_request_mode: function () {
        return Gtk.SizeRequestMode.HEIGHT_FOR_WIDTH;
    },

    vfunc_get_preferred_width: function () {
        return [0, this._natural_width];
    },

    vfunc_get_preferred_height_for_width: function (width) {
        let height = width / this._aspect;
        return [height, height];
    },

    _texture_size_changed: function (texture, width, height) {
        this._natural_width = width;
        this._aspect = width / height;
        this.queue_resize();
    },

    play_file: function(file) {
        this._video_texture.set_uri(file.get_uri());
        this._video_texture.playing = true;
    }
});

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
        let scalable_player = new ScalableGstPlayer();
        scalable_player.play_file(file);
        return scalable_player;
    }
};
