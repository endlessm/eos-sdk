// Copyright 2014 Endless Mobile, Inc.

const Lang = imports.lang;
const Endless = imports.gi.Endless;
const Gtk = imports.gi.Gtk;
const GObject = imports.gi.GObject;

const TEST_APPLICATION_ID = 'com.endlessm.example.test';

const TestContainer = Lang.Class({
    Name: 'TestContainer',
    Extends: Endless.CustomContainer,

    _init: function() {
        this.parent();

        this._frame = new Gtk.Frame();
        this.add(this._frame);
    },

    vfunc_size_allocate: function (alloc) {
        this.parent(alloc);
        alloc.width = alloc.width / 2;
        alloc.height = alloc.height / 2;
        this._frame.size_allocate(alloc);
    }
});

const TestApplication = new Lang.Class ({
    Name: 'TestApplication',
    Extends: Gtk.Application,

    vfunc_startup: function() {
        this.parent();
        let window = new Gtk.Window();
        window.add(new TestContainer());
        window.show_all();
        this.add_window(window);
    }
});

let app = new TestApplication({ application_id: TEST_APPLICATION_ID,
                                flags: 0 });
app.run(ARGV);
