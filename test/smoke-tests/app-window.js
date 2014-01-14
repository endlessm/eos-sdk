// Copyright 2013 Endless Mobile, Inc.

const Lang = imports.lang;
const Endless = imports.gi.Endless;
const Gtk = imports.gi.Gtk;
const Gdk = imports.gi.Gdk;
const GObject = imports.gi.GObject;

const TEST_APPLICATION_ID = 'com.endlessm.example.test';

const DOG_BACKGROUND_PATH = './test/smoke-tests/images/dog_eye.jpg';
const CAT_BACKGROUND_PATH = './test/smoke-tests/images/cat_eye.jpg';

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
        this.button3 = new Gtk.Button({ label: 'Sync page backgrounds' });
        this.add(this.button3);
        this.button4 = new Gtk.Button({ label: 'Crazy background' });
        this.add(this.button4);
        this.button5 = new Gtk.Button({ label: 'Reset background' });
        this.add(this.button5);
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
        this.button1 = new Gtk.Button({ label: 'Scale font down' });
        this.button2 = new Gtk.Button({ label: 'Scale font up' });

        this.add(this._label);
        this.add(this._label1);
        this.add(this.switch1);
        this.add(this._label2);
        this.add(this.switch2);
        this.add(this.button1);
        this.add(this.button2);
    }
});

const LeftTopbar = new Lang.Class ({
    Name: 'LeftTopBar',
    Extends: Gtk.Grid,

    _init: function(props) {
        props = props || {};
        props.orientation = Gtk.Orientation.VERTICAL;
        this.parent(props);

        this._label = new Gtk.Label({ label: 'The Left Topbar' });

        this.add(this._label);
    }
});

const CenterTopbar = new Lang.Class ({
    Name: 'CenterTopBar',
    Extends: Gtk.Grid,

    _init: function(props) {
        props = props || {};
        props.orientation = Gtk.Orientation.VERTICAL;
        this.parent(props);

        this._label = new Gtk.Label({ label: 'The Center Topbar' });

        this.add(this._label);
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
        this._page1.button3.connect('clicked', Lang.bind(this, function () {
            this._setupDogBackground(this._page1);
        }));
        this._page1.button4.connect('clicked', Lang.bind(this, function () {
            this._setupCrazyDogBackground(this._page1);
        }));
        this._page1.button5.connect('clicked', Lang.bind(this, function () {
            this._setupCatBackground(this._page1);
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

        this._left_topbar = new LeftTopbar();

        this._center_topbar = new CenterTopbar();

        this._pm.add(this._page0, {
            name: "page0",
            background_uri: CAT_BACKGROUND_PATH,
            custom_toolbox_widget: this._toolbox,
            left_topbar_widget: this._left_topbar,
            center_topbar_widget: this._center_topbar 
        });

        this._pm.add(this._page1, {
            name: "page1",
            custom_toolbox_widget: this._toolbox,
            left_topbar_widget: this._left_topbar,
            center_topbar_widget: this._center_topbar,
            page_actions: true
        });

        this._setupDogBackground(this._page0);
        this._setupCatBackground(this._page1);

        this._window = new Endless.Window({
            application: this,
            page_manager: this._pm,
            'font-scaling-active': true,
            'font-scaling-default-size': 16
        });

        this._toolbox.button1.connect('clicked', Lang.bind(this, function () {
            let current_font_size = this._window.get_font_scaling_default_size();
            this._window.set_font_scaling_default_size(current_font_size - 1);
        }));
        this._toolbox.button2.connect('clicked', Lang.bind(this, function () {
            let current_font_size = this._window.get_font_scaling_default_size();
            this._window.set_font_scaling_default_size(current_font_size + 1);
        }));

        let provider = new Gtk.CssProvider();
        provider.load_from_data("EosWindow { font-size: 1em; }");
        Gtk.StyleContext.add_provider_for_screen(Gdk.Screen.get_default(), provider,
                                                 Gtk.STYLE_PROVIDER_PRIORITY_APPLICATION);

        this._window.show_all();
    },

    _setupDogBackground: function (page) {
        this._pm.set_page_background_uri(page, DOG_BACKGROUND_PATH);
        this._pm.set_page_background_size(page, "auto");
        this._pm.set_page_background_position(page, "center");
        this._pm.set_page_background_repeats(page, false);
    },

    _setupCrazyDogBackground: function (page) {
        this._pm.set_page_background_uri(page, DOG_BACKGROUND_PATH);
        this._pm.set_page_background_size(page, "100px 100px");
        this._pm.set_page_background_position(page, "0% 0%");
        this._pm.set_page_background_repeats(page, true);
    },

    _setupCatBackground: function (page) {
        this._pm.set_page_background_uri(page, CAT_BACKGROUND_PATH);
        this._pm.set_page_background_size(page, "auto");
        this._pm.set_page_background_position(page, "center");
        this._pm.set_page_background_repeats(page, false);
    },

    _onButtonClicked: function () {
        this._window.destroy();
    }
});

let app = new TestApplication({ application_id: TEST_APPLICATION_ID,
                                flags: 0 });
app.run(ARGV);
