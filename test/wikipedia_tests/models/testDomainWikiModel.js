const DomainWikiModel = imports.wikipedia.models.domain_wiki_model;

describe("Domain Wiki Model", function() {
  const mockJsonData = {
    categories: [
        {
            category_name: 'Main Category',
            content_text: 'Lorem Ipsum',
            image_file: 'file:///image.jpg',
            image_thumb_uri: 'file:///image_thumb.jpg',
            is_main_category: true,
            subcategories: [
                'Category One',
                'Category Two'
            ]
        },
        {
            category_name: 'Category One',
            content_text: 'Lorem Ipsum',
            image_file: 'file:///image.jpg',
            image_thumb_uri: 'file:///image_thumb.jpg',
            is_main_category: false,
            subcategories: []
        },
        {
            category_name: 'Category Two',
            content_text: 'Lorem Ipsum',
            image_file: 'file:///image.jpg',
            image_thumb_uri: 'file:///image_thumb.jpg',
            is_main_category: false,
            subcategories: [
                'Category Three'
            ]
        },
        {
            category_name: 'Category Three',
            content_text: 'Lorem Ipsum',
            image_file: 'file:///image.jpg',
            image_thumb_uri: 'file:///image_thumb.jpg',
            is_main_category: false,
            subcategories: []
        }
    ],
    articles: [
        {
            title: 'Article One',
            url: 'file:///article1.html',
            source: 'Mock data',
            categories: [
                'Category One'
            ]
        },
        {
            title: 'Article Two',
            url: 'file:///article2.html',
            source: 'Mock data',
            categories: [
                'Category One',
                'Category Two'
            ]
        },
        {
            title: 'Article Three',
            url: 'file:///article3.html',
            source: 'Mock data',
            categories: [
                'Category Two'
            ]
        }
    ]
  };

  beforeEach(function() {
    let model = new DomainWikiModel.DomainWikiModel();

    jasmine.addMatchers({
      toHaveObjectsContainingProperties: function() {
        return {
          compare: function(actual, propertyMap) {
            let result = {
              pass: (function() {
                for (let property in propertyMap) {
                  let allValuesListedHaveAMatchForObject = actual.some(function(object) {
                    if (object[property] == 'undefined') {
                      return false;
                    }
                    
                    let propertyValueMatchedForObject =
                      propertyMap[property].some(function(value) {
                        return object[property] == value;
                      });
                    
                    return propertyValueMatchedForObject;
                  });
                  
                  if (!allValuesListedHaveAMatchForObject)
                    return false; 
                }
                
                return true;
              })(),

              message: (function() {
                let msg = "Expected objects to have the following values for the following properties \n";
                for (let property in propertyMap) {
                  msg += " - Property: " + property + "\n";
                  for (let value in propertyMap[property]) {
                    msg += "   * Value: " + propertyMap[property][value].toString() + "\n";
                  }
                }
                
                msg += "Object actually has the following toplevel properties\n";
                
                for (let i = 0; i < actual.length; i++) {
                  let object = actual[i];
                  msg += " Object in position " + i + "\n";
                  for (let property in object) {
                    msg += " - " + property + " : " + object[property] + "\n";
                  }
                }
                
                return msg;
              })()
            }
            
            return result;
          }
        }
      }
    });
  });

  describe("when loaded from some mock JSON data", function() {
    let model;
    beforeEach(function() {
      model = new DomainWikiModel.DomainWikiModel();
      model.loadFromJson(mockJsonData);
    });

    it("returns all articles when getting articles", function() {
      let articles = model.getArticles();
      expect(articles).toHaveObjectsContainingProperties({
        title: [ 'Article One', 'Article Two', 'Article Three' ]
      });
    });

    it("can get articles for a category", function() {
      let articles = model.getArticlesForCategory('Category One');
      expect(articles).toHaveObjectsContainingProperties({
        title: [ 'Article One', 'Article Two' ]
      });
    });

    it("has no articles on a category that does not have articles", function() {
      let articles = model.getArticlesForCategory('Main Category');
      expect(articles.length).toEqual(0);
    });

    // TODO: Shouldn't this throw?
    it("has no articles for a category that does not exist", function() {
      let articles = model.getArticlesForCategory('Nonexistent');
      expect(articles.length).toEqual(0);
    });

    it("can check whether or not a category has articles", function() {
      expect(model._getCategoryHasArticles('Category Two')).toBeTruthy();
    });

    it("can check whether or not a category does not have articles", function() {
      expect(model._getCategoryHasArticles('Category Three')).toBeFalsy();
    });

    // TODO: Again this should throw.
    it("verifies that a category that does not exist has no articles", function() {
      expect(model._getCategoryHasArticles('Nonexistent')).toBeFalsy();
    });

    describe("category fetch", function() {
      let category;

      beforeEach(function() {
        category = model.getCategory('Category One');
      });
      
      it("actually returns a category", function() {
        expect(category.__name__).toEqual('CategoryModel');
      });

      it("returns the right category", function() {
        expect(category.title).toEqual('Category One');
      });
    });

    // TODO: This should throw
    it("returns an undefined value if we try to get a category that doesn't exist", function() {
      expect(model.getCategory('Nonexistent')).toBeUndefined();
    });

    it("returns 'Main Category' when getting the main category", function() {
      let category = model.getMainCategory();

      expect(category).toEqual(new jasmine.ObjectContaining({
        'title' : 'Main Category'
      }));
    });
  });

  it("returns null when the Main Category is unset", function() {
    let model = new DomainWikiModel.DomainWikiModel();
    expect(model.getMainCategory()).toBeNull();
  });
});
