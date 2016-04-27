// Copyright (C) 2015-2016 Endless Mobile, Inc.

const Gio = imports.gi.Gio;
const GLib = imports.gi.GLib;
const Gtk = imports.gi.Gtk;
const WebHelper2 = imports.webhelper2;
const WebKit2 = imports.gi.WebKit2;

const WELL_KNOWN_NAME = 'com.endlessm.WebHelper.testLocal';

Gtk.init(null);

describe('Local URI scheme', function () {
    let owner_id, connection, webview, webhelper;

    beforeAll(function (done) {
        owner_id = Gio.DBus.own_name(Gio.BusType.SESSION, WELL_KNOWN_NAME,
            Gio.BusNameOwnerFlags.NONE,
            null,  // bus acquired
            (con, name) => {  // name acquired
                connection = con;
                done();
            },
            null);  // name lost
    });

    afterAll(function () {
        Gio.DBus.unown_name(owner_id);
    });

    beforeEach(function () {
        webhelper = new WebHelper2.WebHelper({
            well_known_name: WELL_KNOWN_NAME,
            connection: connection,
        });
        webview = new WebKit2.WebView();
    });

    afterEach(function () {
        webhelper.unregister();
    });

    it('loads a local file', function (done) {
        let path = GLib.build_filenamev([GLib.getenv('TOP_SRCDIR'), 'test',
            'tools', 'test.html']);
        let test_file = Gio.File.new_for_path(path);

        let error_spy = jasmine.createSpy('error_spy');
        webview.connect('load-failed', error_spy);
        let id = webview.connect('load-changed', (webview, event) => {
            if (event === WebKit2.LoadEvent.FINISHED) {
                webview.disconnect(id);
                expect(error_spy).not.toHaveBeenCalled();
                done();
            }
        });
        webview.load_uri(test_file.get_uri().replace(/^file:/, 'local:'));
    });
});
