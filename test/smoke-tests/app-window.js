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

        this._faketopbar = new Gtk.EventBox();
        this._faketopbar.get_style_context().add_class("top-bar");
        this._button = new Gtk.Button({ halign: Gtk.Align.CENTER, valign: Gtk.Align.CENTER });
        this._button.add(Gtk.Image.new_from_icon_name("window-close-symbolic", Gtk.IconSize.DIALOG));
        this._faketopbar.add(this._button);

        this._window = new Endless.Window({
            application: this,
            border_width: 16
        });
        this._window.add(this._faketopbar);
        this._window.show_all();
    },

    _onButtonClicked: function () {
        this._window.destroy();
    },
});

Gtk.init(null);
let app = new TestApplication({ application_id: TEST_APPLICATION_ID,
                                flags: 0 });
app.run(ARGV);
