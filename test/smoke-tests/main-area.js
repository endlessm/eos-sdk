// Copyright 2013 Endless Mobile, Inc.

const Lang = imports.lang;
const Endless = imports.gi.Endless;
const Gtk = imports.gi.Gtk;
const Gdk = imports.gi.Gdk;

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
        this._button = new Gtk.Button({ label: 'Hello World!' });
        this._label = new Gtk.Label({ label: 'The toolbar...' });
        this._main_area = new Endless.MainArea();
        this._main_area.set_content(this._button);
        this._main_area.set_toolbar(this._label);
        this._main_area.set_actions(true);
        this._window.add(this._main_area);
        this._window.show_all();
        this._window.connect("key-press-event", Lang.bind(this, this._on_key_press));
    },

    vfunc_activate: function() {
        this.parent();
        this._window.present();
    },

    _on_key_press: function(w, event) {
        let keyval = event.get_keyval()[1];
        if (keyval === Gdk.KEY_l) {
            if(this._main_area.get_toolbar()===null) {
                print("Adding toolbar");
                this._main_area.set_toolbar(this._label);
            }
            else {
                print("Removing toolbar");
                this._main_area.remove(this._label);
            }
        }
        else if (keyval === Gdk.KEY_r) {
            let actions = this._main_area.get_actions();
            this._main_area.set_actions(!actions);
            if (actions) {
                print("Adding actions");
            }
            else {
                print("Removing actions");
            }
        }
        else if (keyval === Gdk.KEY_c) {
            if(this._main_area.get_content()===null) {
                print("Adding content");
                this._main_area.set_content(this._button);
            }
            else {
                print("Removing content");
                this._main_area.remove(this._button);
            }
        }
    }
});

print("l: toggle left toolbar");
print("r: toggle right action area");
print("c: toggle content");
let app = new TestApplication();
app.run(ARGV);
