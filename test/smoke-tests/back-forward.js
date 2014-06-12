// Copyright 2013 Endless Mobile, Inc.

const Lang = imports.lang;
const Endless = imports.gi.Endless;
const Gtk = imports.gi.Gtk;
const Gdk = imports.gi.Gdk;
const GObject = imports.gi.GObject;

const TEST_APPLICATION_ID = 'com.endlessm.example.test-back-forward';

const CSS_STYLES = ' \
    .count { \
        font-size: 36px; \
        padding: 10px; \
        color: black; \
        background-color: white; \
        border-radius: 0; \
        transition: border-radius 500ms, background-color 500ms; \
    } \
    .current { \
        border-radius: 20px; \
        background-color: purple; \
    }';

const Page = new Lang.Class({
    Name: 'Page',
    Extends: Gtk.Grid,
    Properties: {
        'current': GObject.ParamSpec.int('current', '', '',
            GObject.ParamFlags.READABLE | GObject.ParamFlags.WRITABLE,
            0, 4, 0)
    },

    _init: function (props) {
        this._current = 0;
        this._frames = null;

        props = props || {};
        props.orientation = Gtk.Orientation.HORIZONTAL;
        this.parent(props);

        this._frames = [0, 1, 2, 3, 4].map(function (item, index) {
            let frame = new Gtk.Frame({
                margin: 5
            });
            let label = new Gtk.Label({
                label: index.toString()
            });
            frame.add(label);
            frame.get_style_context().add_class('count');
            this.add(frame);
            return frame;
        }, this);
    },

    get current() {
        return this._current;
    },

    set current(value) {
        if(this._frames)
            this._frames[this._current].get_style_context().remove_class('current');
        this._current = value;
        if(this._frames)
            this._frames[this._current].get_style_context().add_class('current');
        this.notify('current');
    }
});

const TestApplication = new Lang.Class ({
    Name: 'TestApplication',
    Extends: Endless.Application,

    vfunc_startup: function () {
        this.parent();

        this._win = new Endless.Window({
            application: this
        });
        let page = new Page({
            halign: Gtk.Align.CENTER,
            valign: Gtk.Align.CENTER,
            expand: true,
        });
        let nav = new Endless.TopbarNavButton();
        this._win.page_manager.add(page, {
            left_topbar_widget: nav
        });

        let provider = new Gtk.CssProvider();
        provider.load_from_data(CSS_STYLES);
        Gtk.StyleContext.add_provider_for_screen(Gdk.Screen.get_default(), provider,
            Gtk.STYLE_PROVIDER_PRIORITY_APPLICATION);

        nav.back_button.connect('clicked', function () {
            page.current--;
        });
        nav.forward_button.connect('clicked', function () {
            page.current++;
        });
        page.connect('notify::current', function () {
            nav.back_button.sensitive = true;
            nav.forward_button.sensitive = true;
            if (page.current === 0)
                nav.back_button.sensitive = false;
            if (page.current === 4)
                nav.forward_button.sensitive = false;
        });

        page.current = 0;
    },

    vfunc_activate: function () {
        this._win.show_all();

        this.parent();
    }
});

let app = new TestApplication({
    application_id: TEST_APPLICATION_ID,
    flags: 0
});
app.run(ARGV);
