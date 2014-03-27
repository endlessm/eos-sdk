// Copyright 2013 Endless Mobile, Inc.

const Lang = imports.lang;
const Gio = imports.gi.Gio;
const Gtk = imports.gi.Gtk;
const Gdk = imports.gi.Gdk;
const Endless = imports.gi.Endless;

const SCRIPT_DIR = Endless.getCurrentFileDir();
imports.searchPath.unshift(SCRIPT_DIR + "/../../");

const ObjectPreviewer = imports.wikipedia.widgets.object_previewer;
const Lightbox = imports.wikipedia.widgets.lightbox;

const TEST_APPLICATION_ID = 'com.endlessm.example.previewer';

const PreviewerApplication = new Lang.Class ({
    Name: 'PreviewerApplication',
    Extends: Gtk.Application,

    vfunc_startup: function () {
        this.parent();

        let grid = new Gtk.Grid({
            orientation: Gtk.Orientation.HORIZONTAL,
            expand: true,
            valign: Gtk.Align.CENTER,
            column_homogeneous: true
        });
        let image_button = new Gtk.Button({
            halign: Gtk.Align.CENTER,
            label: "Da image"
        });
        image_button.connect("clicked", Lang.bind(this, function () {
            let file = Gio.File.new_for_path(SCRIPT_DIR + "/images/dog_eye.jpg");
            this._object_previewer.preview_file(file);
            this._lightbox.show_lightbox();
        }));
        grid.add(image_button);
        let video_button = new Gtk.Button({
            halign: Gtk.Align.CENTER,
            label: "Da video"
        });
        video_button.connect("clicked", Lang.bind(this, function () {
            let file = Gio.File.new_for_path(SCRIPT_DIR + "/videos/sample.mp4");
            this._object_previewer.preview_file(file);
            this._lightbox.show_lightbox();
        }));
        grid.add(video_button);
        let document_button = new Gtk.Button({
            halign: Gtk.Align.CENTER,
            label: "Da document"
        });
        document_button.connect("clicked", Lang.bind(this, function () {
            let file = Gio.File.new_for_path(SCRIPT_DIR + "/pdfs/sample.pdf");
            this._object_previewer.preview_file(file);
            this._lightbox.show_lightbox();
        }));
        grid.add(document_button);
        let alignment = new Gtk.Alignment({
            expand: true
        });
        alignment.add(grid);

        this._object_previewer = new ObjectPreviewer.ObjectPreviewer();

        this._lightbox = new Lightbox.Lightbox();
        this._lightbox.add_lightbox_widget(this._object_previewer);
        this._lightbox.hide_lightbox();
        this._lightbox.add(alignment);

        let window = new Gtk.Window();
        window.add(this._lightbox);
        window.maximize();
        window.show_all();
        this.add_window(window);

        // css
        let provider = new Gtk.CssProvider();
        provider.load_from_path(SCRIPT_DIR + "/object-previewer-lightbox.css");
        Gtk.StyleContext.add_provider_for_screen(Gdk.Screen.get_default(), provider,
                                                 Gtk.STYLE_PROVIDER_PRIORITY_APPLICATION);
    },

    vfunc_activate: function () {
    }
});

ObjectPreviewer.init_plugins();
let app = new PreviewerApplication({ application_id: TEST_APPLICATION_ID,
                                     flags: 0 });
app.run(ARGV);
