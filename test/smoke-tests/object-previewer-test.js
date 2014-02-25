// Copyright 2013 Endless Mobile, Inc.

const Lang = imports.lang;
const Gio = imports.gi.Gio;
const Gtk = imports.gi.Gtk;
const GtkClutter = imports.gi.GtkClutter;
const ClutterGst = imports.gi.ClutterGst;
const EvinceDocument = imports.gi.EvinceDocument;
const Endless = imports.gi.Endless;

imports.searchPath.unshift(Endless.getCurrentFileDir() + "/../../");

const ObjectPreviewer = imports.wikipedia.widgets.object_previewer;

const TEST_APPLICATION_ID = 'com.endlessm.example.previewer';

const PreviewerApplication = new Lang.Class ({
    Name: 'PreviewerApplication',
    Extends: Gtk.Application,

    vfunc_startup: function () {
        this.parent();

        this._object_previewer = new ObjectPreviewer.ObjectPreviewer({
            expand: true
        });

        let window = new Gtk.Window();
        window.add(this._object_previewer);
        window.show_all();

        this.add_window(window);
    },

    vfunc_activate: function () {
    },

    vfunc_open: function (files) {
        this._object_previewer.preview_file(files[0]);
    }
});

ObjectPreviewer.init_plugins();
let app = new PreviewerApplication({ application_id: TEST_APPLICATION_ID,
                                     flags: Gio.ApplicationFlags.HANDLES_OPEN });
// Arg parsing in gjs doesn't seem to be working right. Without this dummy
// variable vfunc_open won't get called
ARGV.unshift("dummy");
app.run(ARGV);
