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
        this._page0 = new Gtk.Grid({ orientation: Gtk.Orientation.VERTICAL });
        let a1 = new Gtk.Button({ label: 'Go to page1' });
        a1.connect('clicked', Lang.bind(this, function () {
            this._pm.visible_page = this._page1;
        }));
        this._page0.add(a1);
        let a2 = new Gtk.Button({ label: 'Go to page named "page1"' });
        a2.connect('clicked', Lang.bind(this, function () {
            this._pm.visible_page_name = "page1";
        }));
        this._page0.add(a2);

        // Second page
        this._page1 = new Gtk.Grid({ orientation: Gtk.Orientation.HORIZONTAL });
        let b1 = new Gtk.Button({ label: 'Go to page0' });
        b1.connect('clicked', Lang.bind(this, function () {
            this._pm.visible_page = this._page0;
        }));
        this._page1.add(b1);
        let b2 = new Gtk.Button({ label: 'Go to page named "page0"' });
        b2.connect('clicked', Lang.bind(this, function () {
            this._pm.visible_page_name = "page0";
        }));
        this._page1.add(b2);

        this._pm = new Endless.PageManager();
        this._pm.add(this._page0, { name: "page0" });
        this._pm.add(this._page1, { name: "page1" });

        this._window = new Endless.Window({
            application: this,
            border_width: 16
        });
        this._window.add(this._pm);
        this._window.show_all();
    },

    _onButtonClicked: function () {
        this._window.destroy();
    },
});

let app = new TestApplication({ application_id: TEST_APPLICATION_ID,
                                flags: 0 });
app.run(ARGV);
