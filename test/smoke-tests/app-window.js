// Copyright 2013 Endless Mobile, Inc.

const Lang = imports.lang;
const Endless = imports.gi.Endless;
const Gtk = imports.gi.Gtk;

const TEST_APPLICATION_ID = 'com.endlessm.example.test';


const Page0 = new Lang.Class ({
    Name: 'Page0',
    Extends: Gtk.Grid,

    _init: function(props) {
        props = props || {};
        props.orientation = Gtk.Orientation.VERTICAL;
        this.parent(props);

        this.button1 = new Gtk.Button({ label: 'Go to page1' });
        this.add(this.button1);
        this.button2 = new Gtk.Button({ label: 'Go to page named "page1"' });
        this.add(this.button2);
    }
});

const Page1 = new Lang.Class ({
    Name: 'Page1',
    Extends: Gtk.Grid,

    _init: function(props) {
        props = props || {};
        props.orientation = Gtk.Orientation.HORIZONTAL;
        this.parent(props);

        this.button1 = new Gtk.Button({ label: 'Go to page0' });
        this.add(this.button1);
        this.button2 = new Gtk.Button({ label: 'Go to page named "page0"' });
        this.add(this.button2);
    }
});

const Toolbox = new Lang.Class ({
    Name: 'Toolbox',
    Extends: Gtk.Grid,

    _init: function(props) {
        props = props || {};
        props.orientation = Gtk.Orientation.VERTICAL;
        this.parent(props);

        this._label = new Gtk.Label({ label: 'The Toolbox' });
        this._label1 = new Gtk.Label({ label: 'Actions on page 0' });
        this._label2 = new Gtk.Label({ label: 'Actions on page 1' });
        this.switch1 = new Gtk.Switch({ active: false });
        this.switch2 = new Gtk.Switch({ active: true });

        this.add(this._label);
        this.add(this._label1);
        this.add(this.switch1);
        this.add(this._label2);
        this.add(this.switch2);
    }
});

const TestApplication = new Lang.Class ({
    Name: 'TestApplication',
    Extends: Endless.Application,

    vfunc_startup: function() {
        this.parent();

        // First page
        this._page0 = new Page0();
        this._page0.button1.connect('clicked', Lang.bind(this, function () {
            this._pm.visible_page = this._page1;
        }));
        this._page0.button2.connect('clicked', Lang.bind(this, function () {
            this._pm.visible_page_name = "page1";
        }));

        // Second page
        this._page1 = new Page1();
        this._page1.button1.connect('clicked', Lang.bind(this, function () {
            this._pm.visible_page = this._page0;
        }));
        this._page1.button2.connect('clicked', Lang.bind(this, function () {
            this._pm.visible_page_name = "page0";
        }));

        this._toolbox = new Toolbox();
        this._toolbox.switch1.connect('notify::active', Lang.bind(this, function () {
            this._pm.set_page_actions(this._page0,
                this._toolbox.switch1.active);
        }));
        this._toolbox.switch2.connect('notify::active', Lang.bind(this, function () {
            this._pm.set_page_actions(this._page1,
                this._toolbox.switch2.active);
        }));

        this._pm = new Endless.PageManager();
        this._pm.add(this._page0, {
            name: "page0",
            background_uri: "./test/smoke-tests/images/cat_eye.jpg",
            custom_toolbox_widget: this._toolbox
        });
        this._pm.add(this._page1, {
            name: "page1",
            background_uri: "./test/smoke-tests/images/dog_eye.jpg",
            custom_toolbox_widget: this._toolbox,
            page_actions: true
        });

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
