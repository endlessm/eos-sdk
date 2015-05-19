const Gio = imports.gi.Gio;
const Gtk = imports.gi.Gtk;
const Lang = imports.lang;
const Mainloop = imports.mainloop;
const WebHelper2 = imports.webhelper2;
const WebKit2 = imports.gi.WebKit2;

const WELL_KNOWN_NAME = 'com.endlessm.WebHelper.testWebActions2';

Gtk.init(null);

describe('WebKit2 actions bindings', function () {
    let owner_id, connection, webview, webhelper, web_action_spy;

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

    function run_loop(action_to_test) {
        let string = '<html><head><meta http-equiv="refresh" content="0;url=' +
            action_to_test + '"></head><body></body></html>';
        webview.load_html(string, null);
        Mainloop.run('webhelper2');
    }

    beforeEach(function () {
        webhelper = new WebHelper2.WebHelper({
            well_known_name: WELL_KNOWN_NAME,
            connection: connection,
        });
        webview = new WebKit2.WebView();
        web_action_spy = jasmine.createSpy('web_action_spy').and.callFake(() =>
            Mainloop.quit('webhelper2'));
        webhelper.define_web_action('action', web_action_spy);
    });

    afterEach(function () {
        webhelper.unregister();
    });

    it('calls a web action', function () {
        run_loop('webhelper://action');
        expect(web_action_spy).toHaveBeenCalled();
    });

    it('calls a web action with a parameter', function () {
        run_loop('webhelper://action?param=value');
        expect(web_action_spy).toHaveBeenCalledWith(jasmine.objectContaining({
            param: 'value',
        }));
    });

    it('calls a web action with many parameters', function () {
        run_loop('webhelper://action?first=thefirst&second=thesecond&third=thethird');
        expect(web_action_spy).toHaveBeenCalledWith(jasmine.objectContaining({
            first: 'thefirst',
            second: 'thesecond',
            third: 'thethird',
        }));
    });

    it('uri-decodes parameter names', function () {
        run_loop('webhelper://action?p%C3%A4r%C3%A4m%F0%9F%92%A9=value');
        expect(web_action_spy).toHaveBeenCalledWith(jasmine.objectContaining({
            'päräm💩': 'value',
        }));
    });

    it('uri-decodes parameter values', function () {
        run_loop('webhelper://action?param=v%C3%A1lu%C3%A9%F0%9F%92%A9');
        expect(web_action_spy).toHaveBeenCalledWith(jasmine.objectContaining({
            param: 'válué💩',
        }));
    });

    // This is commented out because GJS cannot catch exceptions across FFI
    // interfaces (e.g. in GObject callbacks.)
    xit('raises an exception on a nonexistent action instead of calling it', function () {
        expect(function () {
            run_loop('webhelper://nonexistentAction?param=value');
        }).toThrow();
    });

    it('calls a web action with a blank parameter', function () {
        run_loop('webhelper://action?param=');
        expect(web_action_spy).toHaveBeenCalledWith(jasmine.objectContaining({
            param: '',
        }));
    });

    it('uri-decodes web action names', function () {
        webhelper.define_web_action('äction💩Quit', web_action_spy);
        run_loop('webhelper://%C3%A4ction%F0%9F%92%A9Quit');
        expect(web_action_spy).toHaveBeenCalled();
    });

    it('can define more than one action with define_web_actions()', function () {
        webhelper.define_web_actions({
            action2: web_action_spy,
        });
        run_loop('webhelper://action2');
        expect(web_action_spy).toBeTruthy();
    });

    it('complains when defining an action that is not a function', function () {
        expect(function () {
            webhelper.define_web_action('badAction', 'not a function');
        }).toThrow();
    });
});
