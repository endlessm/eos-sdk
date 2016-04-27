// Copyright (C) 2016 Endless Mobile, Inc.

const Endless = imports.gi.Endless;
const GdkPixbuf = imports.gi.GdkPixbuf;
const Gio = imports.gi.Gio;
const Gtk = imports.gi.Gtk;
const Lang = imports.lang;

const TestApp = new Lang.Class({
    Name: 'TestApp',
    Extends: Endless.Application,

    vfunc_startup: function () {
        this.parent();
        let win = new TestWindow({ application: this });
        win.show_all();
    }
});

const TestWindow = new Lang.Class({
    Name: 'TestWindow',
    Extends: Endless.Window,

    _init: function (props) {
        this.parent(props);

        let grid = new Gtk.Grid({ orientation: Gtk.Orientation.HORIZONTAL });
        ['test1', 'test2', 'test3', 'Fahrradrheinpromenade'].forEach((key) => {
            let pixbuf = GdkPixbuf.Pixbuf.new_from_resource_at_scale('/com/example/attributiontest/' + key + '.jpg',
                200, -1, true);
            let image = Gtk.Image.new_from_pixbuf(pixbuf);
            grid.add(image);
        });
        this.page_manager.add(grid);
    }
});

let resource = Gio.Resource.load(Endless.getCurrentFileDir() + '/images/credits.gresource');
resource._register();

let credits = Gio.File.new_for_uri('resource:///com/example/attributiontest/attribution.json');
let app = new TestApp({
    application_id: 'com.example.attribution',
    image_attribution_file: credits
});
app.run(ARGV);
