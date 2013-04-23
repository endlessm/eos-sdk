// Copyright 2013 Endless Mobile, Inc.

const Lang = imports.lang;
const Endless = imports.gi.Endless;
const Gtk = imports.gi.Gtk;

const TEST_APPLICATION_ID = 'com.endlessm.example.test';

const TestApplication = new Lang.Class ({
    Name: 'TestApplication',
    Extends: Gtk.Application,

    vfunc_startup: function() {
        this.parent();
        this._window = new Gtk.ApplicationWindow({
            application: this,
            default_height: 400,
            default_width: 600,
            window_position: Gtk.WindowPosition.CENTER });
        this._label = new Gtk.Label({ label: 'Hello World' });
        this._main_area = new Endless.MainArea();
        this._main_area.set_content(this._label);
        this._window.add(this._main_area);
        this._window.show_all();
    },

    vfunc_activate: function() {
        this.parent();
        this._window.present();
    }
});

let app = new TestApplication();
app.run(ARGV);
