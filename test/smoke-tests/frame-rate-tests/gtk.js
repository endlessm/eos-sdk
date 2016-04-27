// Copyright (C) 2013-2016 Endless Mobile, Inc.

const Lang = imports.lang;
const Gtk = imports.gi.Gtk;
const GObject = imports.gi.GObject;
const Endless = imports.gi.Endless;

const TEST_APPLICATION_ID = "com.frametest";

let TRANSITION1 = Gtk.StackTransitionType.SLIDE_LEFT;
let TRANSITION2 = Gtk.StackTransitionType.SLIDE_RIGHT;
if (ARGV[0] === "crossfade") {
    TRANSITION1 = Gtk.StackTransitionType.CROSSFADE;
    TRANSITION2 = Gtk.StackTransitionType.CROSSFADE;
}

let BACKGROUND1_NAME = "page1_1080";
let BACKGROUND2_NAME = "page2_1080";
if (ARGV[1] === "720") {
    BACKGROUND1_NAME = "page1_720";
    BACKGROUND2_NAME = "page2_720";
}

const TestApplication = new Lang.Class ({
    Name: "TestApplication",
    Extends: Gtk.Application,

    vfunc_startup: function() {
        this.parent();

        // First page
        this._page1 = new Gtk.EventBox({
            name: BACKGROUND1_NAME,
            expand: true
        });
        this._page1.connect("button-press-event", Lang.bind(this, function () {
            this._stack.set_transition_type(TRANSITION1);
            this._stack.set_visible_child(this._page2);
        }));

        // Second page
        this._page2 = new Gtk.Button({
            name: BACKGROUND2_NAME,
            expand: true
        });
        this._page2.connect("button-press-event", Lang.bind(this, function () {
            this._stack.set_transition_type(TRANSITION2);
            this._stack.set_visible_child(this._page1);
        }));

        this._stack = new Gtk.Stack();
        this._stack.add(this._page1);
        this._stack.add(this._page2);

        this._stack.set_transition_duration(10000);

        this._window = new Gtk.Window({
            application: this,
        });
        this._window.add(this._stack);
        this._window.show_all();
        this._window.fullscreen();
        this._window.connect("key-press-event", Lang.bind(this, function() {
            this._window.destroy();
        }));

        let provider = new Gtk.CssProvider ();
        provider.load_from_path (Endless.getCurrentFileDir() + "/gtk.css");
        let context = new Gtk.StyleContext();
        context.add_provider_for_screen(this._window.get_screen(),
                                        provider,
                                        Gtk.STYLE_PROVIDER_PRIORITY_USER);

    },

    vfunc_activate: function() {
        this.parent();
    }
});

let app = new TestApplication({ application_id: TEST_APPLICATION_ID,
                                flags: 0 });
app.run(ARGV);
