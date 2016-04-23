// Copyright 2016 Endless Mobile, Inc.

const Clutter = imports.gi.Clutter;
const GdkPixbuf = imports.gi.GdkPixbuf;
const Cogl = imports.gi.Cogl;
const Lang = imports.lang;
const Endless = imports.gi.Endless;
Clutter.init(null, null);

let crossfade = ARGV[0] === "crossfade";

let file_dir = Endless.getCurrentFileDir();

let BACKGROUND1_PATH = file_dir + '/1080/background1.jpg';
let BACKGROUND2_PATH = file_dir + '/1080/background2.jpg';
if (ARGV[1] === "720") {
    BACKGROUND1_PATH = file_dir + '/720/background1.jpg';
    BACKGROUND2_PATH = file_dir + '/720/background2.jpg';
}

// Convenience function to load a gresource image into a Clutter.Image
function load_clutter_image(path) {
    let pixbuf = GdkPixbuf.Pixbuf.new_from_file(path);
    let image = new Clutter.Image();
    if (pixbuf != null) {
        image.set_data(pixbuf.get_pixels(),
                       pixbuf.get_has_alpha()
                           ? Cogl.PixelFormat.RGBA_8888
                           : Cogl.PixelFormat.RGB_888,
                       pixbuf.get_width(),
                       pixbuf.get_height(),
                       pixbuf.get_rowstride());
    }
    return image;
}

const TestStage = new Lang.Class ({
    Name: 'TestStage',
    Extends: Clutter.Stage,

    _init: function(params) {
        this.parent(params);
        this.set_fullscreen(true);
        this.connect("allocation-changed", Lang.bind(this, function(actor, allocation) {
            this._update_actors();
        }));

        this._page1 = new Clutter.Actor({
            "x-expand": true,
            "y-expand": true,
            "content": load_clutter_image(BACKGROUND1_PATH),
            "reactive": true
        });
        this.add_child(this._page1);
        this._page1.connect("button-press-event", Lang.bind(this, function(actor, event) {
            this._swap();
        }));

        this._page2 = new Clutter.Actor({
            "x-expand": true,
            "y-expand": true,
            "content": load_clutter_image(BACKGROUND2_PATH),
            "reactive": true
        });
        this.add_child(this._page2);
        this._page2.connect("button-press-event", Lang.bind(this, function(actor, event) {
            this._swap();
        }));

        this._update_actors();
        this.connect("key-press-event", Clutter.main_quit);
    },

    _swap: function() {
        this._swapped = !this._swapped;
        this._page1.save_easing_state();
        this._page2.save_easing_state();
        this._page1.set_easing_duration(10000);
        this._page2.set_easing_duration(10000);
        this._update_actors();
        this._page1.restore_easing_state();
        this._page2.restore_easing_state();
    },

    _update_actors: function() {
        this._page1.x = 0;
        this._page1.width = this.get_width();
        this._page1.height = this.get_height();
        this._page2.x = 0;
        this._page2.width = this.get_width();
        this._page2.height = this.get_height();
        if (crossfade) {
            if(this._swapped)
                this._page2.opacity = 255;
            else
                this._page2.opacity = 0;
        }
        else {
            if(this._swapped)
                this._page1.x = -this.get_width();
            else
                this._page2.x = this.get_width();
        }
    }
});


let stage = new TestStage();
stage.show();
Clutter.main();
stage.destroy();
