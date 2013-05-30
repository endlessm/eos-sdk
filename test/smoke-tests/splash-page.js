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

        // First page
        this._splash_page = new Gtk.Grid({ orientation: Gtk.Orientation.VERTICAL });
        let splash_label = new Gtk.Label({ label: 'The Splash Page' });
        this._splash_page.add(splash_label);
        let splash_button = new Gtk.Button({ label: 'Show main page' });
        splash_button.connect('clicked', Lang.bind(this, function () {
            this._pm.show_main_page();
        }));
        this._splash_page.add(splash_button);

        // Second page
        this._main_page = new Gtk.Grid({ orientation: Gtk.Orientation.HORIZONTAL });
        let main_label = new Gtk.Label({ label: 'The Main Page' });
        this._main_page.add(main_label);
        let main_button = new Gtk.Button({ label: 'Go back to splash page' });
        main_button.connect('clicked', Lang.bind(this, function () {
            this._pm.show_splash_page();
        }));
        this._main_page.add(main_button);

        this._pm = new Endless.SplashPageManager();
        this._pm.add(this._splash_page, {name: "splash"});
        this._pm.add(this._main_page, {name: "main"});
        this._pm.set_splash_page(this._splash_page);
        this._pm.set_main_page(this._main_page);

        this._window = new Endless.Window({
            application: this,
            border_width: 16,
            page_manager: this._pm
        });
        this._window.show_all();
    },

    _onButtonClicked: function () {
        this._window.destroy();
    },
});

let app = new TestApplication({ application_id: TEST_APPLICATION_ID,
                                flags: 0 });
app.run(ARGV);
