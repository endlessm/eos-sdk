// Copyright 2015 Endless Mobile, Inc.

const Gio = imports.gi.Gio;
const Gtk = imports.gi.Gtk;
const Lang = imports.lang;
const Mainloop = imports.mainloop;
const WebHelper2 = imports.webhelper2;
const WebKit2 = imports.gi.WebKit2;

const WELL_KNOWN_NAME = 'com.endlessm.WebHelper.testTranslate2';

Gtk.init(null);

describe('WebHelper2 translator', function () {
    let webhelper, owner_id, connection;

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
    });

    afterEach(function () {
        webhelper.unregister();
    });

    it('complains about a bad gettext function', function () {
        expect(function () {
            webhelper.set_gettext('I am not a function');
        }).toThrow();
    });

    it('gets and sets the gettext function', function () {
        let translation_function = (s) => s;
        webhelper.set_gettext(translation_function);
        expect(webhelper.get_gettext()).toBe(translation_function);
    });

    it('has a null gettext function by default', function () {
        expect(webhelper.get_gettext()).toBeNull();
    });

    it('can remove the gettext function by setting null', function () {
        webhelper.set_gettext((s) => s);
        expect(webhelper.get_gettext()).not.toBeNull();
        webhelper.set_gettext(null);
        expect(webhelper.get_gettext()).toBeNull();
    });

    describe('translating a page', function () {
        let webview;

        function run_loop() {
            webview.connect('load-changed', (webview, event) => {
                if (event === WebKit2.LoadEvent.FINISHED) {
                    webhelper.translate_html(webview, null, (obj, res) => {
                        webhelper.translate_html_finish(res);
                        Mainloop.quit('webhelper2');
                    });
                }
            });
            webview.load_html('<html><body><p name="translatable">Translate Me</p></body></html>',
                null);
            Mainloop.run('webhelper2');
        }

        beforeEach(function () {
            webview = new WebKit2.WebView();
        });

        it('translates a string', function () {
            let gettext_spy = jasmine.createSpy('gettext_spy').and.callFake((s) => s);
            webhelper.set_gettext(gettext_spy);
            run_loop();
            expect(gettext_spy).toHaveBeenCalledWith('Translate Me');
        });

        // The following test is disabled because GJS cannot catch exceptions
        // across FFI interfaces (e.g. in GObject callbacks.)
        xit('complains about a gettext function not being set', function () {
            expect(function () {
                run_loop();
            }).toThrow();
        });

        it('can cancel the translation operation', function (done) {
            webhelper.set_gettext((s) => s);
            webview.connect('load-changed', (webview, event) => {
                if (event === WebKit2.LoadEvent.FINISHED) {
                    let cancellable = new Gio.Cancellable();
                    cancellable.cancel();
                    webhelper.translate_html(webview, cancellable, (obj, res) => {
                        expect(function () {
                            webhelper.translate_html_finish(res);
                        }).toThrow();
                        done();
                    });
                }
            });
            webview.load_html('<html><body></body></html>', null);
        });
    });
});
