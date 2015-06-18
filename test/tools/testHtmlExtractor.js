const Gio = imports.gi.Gio;
const GLib = imports.gi.GLib;

const EXPECTED_OUTPUT = '#line 5 "test/tools/test.html"\n\
_("Finance Builder");\n\
#line 12 "test/tools/test.html"\n\
// TRANSLATORS: This is a test of UTF-8 encoded characters\n\
_("My Bü∂get");\n\
#line 13 "test/tools/test.html"\n\
_("Choose a template");\n\
#line 21 "test/tools/test.html"\n\
_("This is a string that is spread over multiple lines, but that doesn\'t matter to HTML.");\n\
#line 22 "test/tools/test.html"\n\
_("String with a \\"quote\\"");\n';

describe('eos-html-extractor', function () {
    it('works correctly at a minimum', function () {
        let srcdir = GLib.getenv('TOP_SRCDIR');
        if (!srcdir)
            srcdir = '.';
        let executable = GLib.build_filenamev([srcdir,
            'tools/eos-html-extractor']);
        let operand = GLib.build_filenamev([srcdir, 'test/tools/test.html']);
        let process = new Gio.Subprocess({
            argv: [executable, operand, srcdir],
            flags: Gio.SubprocessFlags.STDOUT_PIPE | Gio.SubprocessFlags.STDERR_MERGE,
        });
        process.init(null);
        let [success, stdout] = process.communicate_utf8(null, null);
        expect(process.get_if_exited()).toBeTruthy();
        expect(process.get_exit_status()).toBe(0);
        expect(stdout).toEqual(EXPECTED_OUTPUT);
    });
});
