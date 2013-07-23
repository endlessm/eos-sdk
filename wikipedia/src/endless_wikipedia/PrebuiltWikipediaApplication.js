const Endless = imports.gi.Endless;
const Format = imports.format;
const Lang = imports.lang;
const Gtk = imports.gi.Gtk;

const EndlessWikipedia = imports.endless_wikipedia.EndlessWikipedia;
const DomainWikiView = imports.views.domain_wiki_view;
const DomainWikiModel = imports.models.domain_wiki_model;
const DomainWikiPresenter = imports.presenters.domain_wiki_presenter;

const _ = function(x) { return x; };

String.prototype.format = Format.format;

const PrebuiltWikipediaApplication = new Lang.Class({
    Name: 'PrebuiltWikipediaApplication',
    Extends: EndlessWikipedia.WikipediaApplication,

    _init: function(props) {
        this.parent(props);
    },

    vfunc_startup: function() {
        this.parent();
        this._domain_wiki_view = new DomainWikiView.DomainWikiView(this);
        let filename = this.application_uri;
        this._domain_wiki_presenter = new DomainWikiPresenter.DomainWikiPresenter(this._domain_wiki_model, this._domain_wiki_view, filename);
    }
});
