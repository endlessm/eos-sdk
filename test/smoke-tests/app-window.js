// Copyright 2013 Endless Mobile, Inc.

const Lang = imports.lang;
const Endless = imports.gi.Endless;
const Gtk = imports.gi.Gtk;

const TEST_APPLICATION_ID = 'com.endlessm.example.test';

const TestApplication = new Lang.Class ({
    Name: 'TestApplication',
    Extends: Endless.Application,

    vfunc_startup: function() {
        this.parent();

        this._button = new Gtk.Button({label: 'Close me'});
        this._button.connect('clicked', Lang.bind(this, this._onButtonClicked));

        this._window = new Endless.Window({
            application: this,
            border_width: 16
        });
        this._window.add(this._button);
        this._window.show_all();
    },

    _onButtonClicked: function () {
        this._window.destroy();
    },
});

let app = new TestApplication({ application_id: TEST_APPLICATION_ID,
                                flags: 0 });
app.run(ARGV);
