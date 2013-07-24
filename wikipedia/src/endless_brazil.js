const Endless = imports.gi.Endless;
const Gdk = imports.gi.Gdk;
const Gio = imports.gi.Gio;
const Gtk = imports.gi.Gtk;

// Local imports
imports.searchPath.unshift(Endless.getCurrentFileDir());
const EndlessWikipedia = imports.endless_wikipedia.EndlessWikipedia;

// Load GResource bundle
let resource = Gio.Resource.load(Endless.getCurrentFileDir() + '/../data/endless_brazil.gresource');
resource._register();

let app = new EndlessWikipedia.PrebuiltWikipediaApplication({
    application_uri: 'resource:///com/endlessm/brazil/brazil_categories.json'
});
app.run(ARGV);
