// Copyright 2013 Endless Mobile, Inc.

const Lang = imports.lang;
const Endless = imports.gi.Endless;
const Gtk = imports.gi.Gtk;
const GObject = imports.gi.GObject;

const TEST_APPLICATION_ID = 'com.endlessm.example.test';


const Page0 = new Lang.Class ({
    Name: 'Page0',
    Extends: Gtk.Grid,

    _init: function(pm, props) {
        props = props || {};
        props.orientation = Gtk.Orientation.VERTICAL;
        this.parent(props);

        this.button1 = new Gtk.Button({ label: 'Go to page1' });
        this.add(this.button1);
        this.button2 = new Gtk.Button({ label: 'Go to page named "page1"' });
        this.add(this.button2);

        this._addTransitionOptions(pm);
    },

    _addTransitionOptions: function(pm) {
        // Combo box for transition type...
        let typeMenu = new Gtk.ComboBoxText();
        let type_options = {
            "Transition type: None": Endless.PageManagerTransitionType.NONE,
            "Transition type: Fade": Endless.PageManagerTransitionType.CROSSFADE,
            "Transition type: Slide Right": Endless.PageManagerTransitionType.SLIDE_RIGHT,
            "Transition type: Slide Left": Endless.PageManagerTransitionType.SLIDE_LEFT,
            "Transition type: Slide Down": Endless.PageManagerTransitionType.SLIDE_DOWN,
            "Transition type: Slide Up": Endless.PageManagerTransitionType.SLIDE_UP
        }
        for (let key in type_options) {
            typeMenu.append_text(key);
        }
        typeMenu.set_active (0);
        this.add(typeMenu);
        typeMenu.connect('changed', Lang.bind(this, function () {
            let activeKey = typeMenu.get_active_text();
            pm.set_transition_type(type_options[activeKey]);
        }));
        // Combo box for transition time...
        let durationMenu = new Gtk.ComboBoxText();
        let duration_options = {
            "Transition time: 200": 200,
            "Transition time: 500": 500,
            "Transition time: 1000": 1000,
            "Transition time: 2000": 2000
        }
        for (let key in duration_options) {
            durationMenu.append_text(key);
        }
        durationMenu.set_active (0);
        this.add(durationMenu);
        durationMenu.connect('changed', Lang.bind(this, function () {
            let activeKey = durationMenu.get_active_text();
            pm.set_transition_duration(duration_options[activeKey]);
        }));
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

        this._pm = new Endless.PageManager();

        // First page
        this._page0 = new Page0(this._pm);
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
    }
});

let app = new TestApplication({ application_id: TEST_APPLICATION_ID,
                                flags: 0 });
app.run(ARGV);
