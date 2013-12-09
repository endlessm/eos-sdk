const Gio = imports.gi.Gio;
const GLib = imports.gi.GLib;
const System = imports.system;

const DEFAULT_LOCALE = 'en';
const CURRENT_MANIFEST_VERSION = 1;

function execute(args) {
    let [id, argdict] = parseArgs(args);
    let manifest = createManifest(id, argdict);
    let json_manifest = JSON.stringify(manifest, null, "  ");

    let filename = createFilenameForManifest(manifest);
    let cwd = Gio.File.new_for_path(GLib.get_current_dir());
    let file = cwd.get_child(filename);

    file.replace_contents(json_manifest, null /* etag */,
        false /* make backup */, Gio.FileCreateFlags.REPLACE_DESTINATION,
        null /* cancellable */);
}

// Takes a manifest with a valid application ID and creates the appropriate
// filename to put it in.
function createFilenameForManifest(manifest) {
    return manifest.applicationId + '.json';
}

// Takes a valid application ID and validated list of command-line arguments
// and creates a valid application manifest.
function createManifest(id, argdict) {
    let locale = argdict.locale || DEFAULT_LOCALE;
    let manifestVersion = CURRENT_MANIFEST_VERSION;
    if ('manifest-version' in argdict)
        manifestVersion = parseInt(argdict['manifest-version'], 10);

    let launcherKey, launcherValue;
    if (typeof argdict.appclass != 'undefined') {
        launcherKey = 'applicationClass';
        launcherValue = argdict.appclass;
    } else {
        launcherKey = 'exec';
        launcherValue = argdict.exec;
    }

    let manifest = {};
    manifest.manifestVersion = manifestVersion;
    manifest.applicationId = id;
    manifest.applicationName = createLocaleDict(locale, argdict.appname);
    manifest.authorName = argdict.author;
    manifest.authorWebsite = argdict.website || '';
    manifest.description = createLocaleDict(locale, argdict.description || '');
    manifest.version = argdict.appversion;
    manifest.changes = createLocaleDict(locale, []);
    manifest.license = argdict.license || '';
    manifest.resources = [];
    manifest[launcherKey] = launcherValue;
    manifest.icons = {};
    manifest.categories = [];
    manifest.permissions = [];
    manifest.metadata = {};

    return manifest;
}

// Takes a locale and a JS object and creates a dictionary such that for
// locale == 'pt_BR', return value is { 'pt_BR': value }.
function createLocaleDict(locale, value) {
    let retval = {};
    retval[locale] = value;
    return retval;
}

// Parse command line arguments and return a valid application ID and a
// dictionary of other parameters passed. Validates the parameters.
function parseArgs(args) {
    if (typeof args == 'undefined' || args.length === 0) {
        help();
        throw new Error();
    }

    // App ID is the mandatory first argument
    let id = args.shift();
    if (!Gio.Application.id_is_valid(id))
        throw new Error('"%s" is not a valid application ID. Rules:\n\
- must contain only the ASCII characters "[A-Z][a-z][0-9]_-." and must not\n\
  begin with a digit.\n\
- must contain at least one "."" (period) character (and thus at least three\n\
  characters).\n\
- must not begin or end with a "." (period) character.\n\
- must not contain consecutive "."" (period) characters.\n\
- must not exceed 255 characters.'.format(id));

    let argdict = parseRemainingArgs(args);
    if (!validateRemainingArgs(argdict)) {
        help();
        throw new Error();
    }

    return [id, argdict];
}

// Parse all arguments after the application ID has been removed
function parseRemainingArgs(args) {
    let retval = {};
    for (let count = 0; count < args.length; count++) {
        if (args[count].startsWith('--')) {
            if (args[count].indexOf('=') != -1) {
                // One parameter of the form --parameter=value
                let expr = args[count].slice(2);
                let [parameter, value] = expr.split('=');
                retval[parameter] = value;
                continue;
            }
            // Otherwise, the --parameter consumes the next argument if it's
            // there and not also a --parameter
            if (typeof args[count + 1] == 'undefined' ||
                args[count + 1].startsWith('--')) {
                let parameter = args[count].slice(2);
                retval[parameter] = null;
                continue;
            }
            let parameter = args[count].slice(2);
            retval[parameter] = args[count + 1];
            count++;
        }
    }
    return retval;
}

// Validate all arguments besides the application ID
function validateRemainingArgs(args) {
    let requiredArgs = ['appname', 'author', 'appversion'];
    let knownArgs = ['appclass', 'exec', 'manifest-version', 'website',
        'description', 'locale', 'license'];

    for (let arg of requiredArgs) {
        if (!(arg in args))
            return false;
    }

    // Only one of --appclass or --exec
    if ('appclass' in args && 'exec' in args)
        return false;
    if (!('appclass' in args) && !('exec' in args))
        return false;

    for (let arg in args) {
        if (requiredArgs.indexOf(arg) == -1 && knownArgs.indexOf(arg) == -1)
            return false;
        if (args[arg] === null)
            return false;
    }

    // Validate any content requirements for certain parameters here

    if ('manifest-version' in args) {
        let versionNumber = parseInt(args['manifest-version'], 10);
        if (Number.isNaN(versionNumber) || versionNumber < 0 ||
            versionNumber > CURRENT_MANIFEST_VERSION)
            return false;
    }

    return true;
}

function summary() {
    return 'Generate a minimal valid manifest';
}

function help() {
    print('Generates a minimal valid manifest in the current directory.\n\n\
Usage: %s init <application_id>\n\
           --appname=<name> --appversion=<version>\n\
           --author=<author> {--appclass=<application_class>|--exec=<path>}\n\
           [--manifest-version=<version>] [--website=<website>]\n\
           [--description=<description>] [--locale=<locale>]\n\
           [--license=<license>]\n\n\
Required options for a minimal valid manifest:\n\
  <application_id>   - A unique application ID, e.g. "com.example.weather"\n\
  --appname          - Human-readable application name, e.g. "Weather Reader"\n\
  --author           - Author name, e.g. "Joe Coder <coder@example.com>"\n\
  --appversion       - Application version number\n\
  --appclass         - Name of a class derived from Endless.Application with\n\
                       its module to import, e.g. WeatherReader.App\n\
  --exec             - Path to an executable within the package, that\n\
                       launches the application, e.g. "bin/weather-reader"\n\
(Use only one of either --appclass or --exec.)\n\n\
Other options:\n\
  --manifest-version - Version of the manifest file specification to use\n\
                       [default: the current one]\n\
  --website          - URI with more information about the application\n\
  --description      - Long description of the application\n\
  --locale           - Locale information for --appname and --description\n\
                       [default "en"]\n\
  --license          - Software license under which the application is\n\
                       provided to users'.format(System.programInvocationName));
}
