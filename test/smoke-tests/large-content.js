// Copyright 2013 Endless Mobile, Inc.

const Lang = imports.lang;
const Endless = imports.gi.Endless;
const Gdk = imports.gi.Gdk;
const Gtk = imports.gi.Gtk;
const GObject = imports.gi.GObject;

const TEST_APPLICATION_ID = 'com.endlessm.example.test';

const TestApplication = new Lang.Class ({
    Name: 'TestApplication',
    Extends: Endless.Application,

    vfunc_startup: function() {
        this.parent();

        let big_button = new Gtk.Button({
            label: "BIG OL BUTTON"
        });
        big_button.override_background_color(Gtk.StateFlags.NORMAL,
            new Gdk.RGBA({ red: 0, green: 0, blue: 1, alpha: 1 }));
        big_button.override_background_color(Gtk.StateFlags.ACTIVE,
            new Gdk.RGBA({ red: 1, green: 0, blue: 0, alpha: 1 }));
        big_button.set_size_request(3000, 2000);

        let window = new Endless.Window({
            application: this
        });
        window.get_page_manager().add(big_button);
        // window.connect("size-allocate", function(w, alloc) {
        //     print("Win width", alloc.width);
        //     print("Win height", alloc.height);
        // });

        window.show_all();
    }
});

let app = new TestApplication({ application_id: TEST_APPLICATION_ID,
                                flags: 0 });
app.run(ARGV);
