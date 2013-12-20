const CategoryModel = imports.wikipedia.models.category_model;

describe("Category Model", function() {
    let mockJsonData = {
        category_name: 'Category Name',
        content_text: 'Lorem Ipsum',
        image_file: 'file:///image.jpg',
        image_thumb_uri: 'file:///thumb.jpg',
        is_main_category: false,
        subcategories: [ 'Category Two' ]
    };
    describe("from JSON", function() {

        let model;
        beforeEach(function() {
            model = CategoryModel.newFromJson(mockJsonData);
        });

        it("is a CategoryModel", function() {
            expect(model instanceof CategoryModel.CategoryModel).toBeTruthy();
        });

        it("has an id", function() {
            expect(model.id).toEqual(mockJsonData.category_name);
        });

        it("has no subcategories", function() {
            expect(model.getSubcategories().length).toEqual(0);
        });
    });

    describe("from properties", function() {
        let model;
 
        beforeEach(function() {
            model = new CategoryModel.CategoryModel({
                id: 'id',
                title: 'title',
                description: 'description',
                image_uri: 'image-uri',
                image_thumbnail_uri: 'image-thumbnail-uri',
                is_main_category: true,
                has_articles: true
            });
        });

        it("has an id", function() {
            expect(model.id).toEqual('id');
        });

        it("has a title", function() {
            expect(model.title).toEqual('title');
        });

        it("has a description", function() {
            expect(model.description).toEqual('description');
        });

        it("has an image uri", function() {
            expect(model.image_uri).toEqual('image-uri');
        });

        it("has an image thumbnail uri", function() {
            expect(model.image_thumbnail_uri).toEqual('image-thumbnail-uri');
        });

        it("is a main category", function() {
            expect(model.is_main_category).toBeTruthy();
        });

        it("has articles", function() {
            expect(model.has_articles).toBeTruthy();
        });

        // FIXME: This seems to be a fairly useless test. Does it actually
        // test anything?
        it("does not have articles once the flag is unset", function() {
            model.has_articles = false;
            expect(model.has_articles).toBeFalsy();
        });
    });

    it("starts with no subcategories", function() {
        let model = new CategoryModel.CategoryModel();

        expect(model.getSubcategories().length).toEqual(0);
    });

    describe("in a tree-like structure", function() {
        let parent;

        beforeEach(function() {
            jasmine.addMatchers({
                toContainCategoriesWithNames: function() {
                    return {
                        compare: function(actual, names) {
                            let result = {
                                pass: (function() {
                                    let outer_pass = true;
                                    names.forEach(function (id) {
                                        let categories = actual.getSubcategories();
                                        if (!categories.some(function(category) {
                                            return category.id == id;
                                        })) {
                                            outer_pass = false;
                                        }
                                    });
                                    return outer_pass;
                                })(),

                                message: (function() {
                                    let msg = "Expected categories with the following names\n";
                                    names.forEach(function(name) {
                                        msg += " " + name + "\n";
                                    });
                                    msg += "Object actually has the following categories\n";
                                    actual.getSubcategories().forEach(function(category) {
                                        msg += " " + category.id + "\n";
                                    });
                                    return msg;
                                })();
                            }
                            
                            return result;
                        }
                    }
                },
                toHaveOnlyTheFollowingCategoriesInOrder: function() {
                    return {
                        compare: function(actual, names) {
                            let result = {
                                pass: (function() {
                                    let categories = actual.getSubcategories();
                                    if (categories.length != names.length)
                                        return false;
                                    
                                    for (let i = 0; i < categories.length; i++) {
                                        if (categories[i].id != names[i])
                                            return false;
                                    }
                                    
                                    return true;
                                })(),

                                message: (function() {
                                    let msg = "Expected exactly the following category names\n";
                                    names.forEach(function(name) {
                                        msg += " " + name + "\n";
                                    });
                                    
                                    msg += "Actually had the following category names\n";
                                    actual.getSubcategories().forEach(function(category) {
                                        msg += " " + category.id + "\n";
                                    });
                                    
                                    return msg;
                                })()
                            }
                            
                            return result;
                        }
                    }
                }
            });
            
            parent = new CategoryModel.CategoryModel({ id: 'Category One' });
            parent.addSubcategory(new CategoryModel.CategoryModel({ id: 'Category Two' }));
            parent.addSubcategory(new CategoryModel.CategoryModel({ id: 'Category Three' }));
        });

        it("has subcategories", function() {
            expect(parent).toContainCategoriesWithNames(['Category Two', 'Category Three']);
        });

        it("silently does not add duplicates", function() {
            parent.addSubcategory(new CategoryModel.CategoryModel({ id: 'Category Two' }));
            expect(parent).toHaveOnlyTheFollowingCategoriesInOrder(['Category Two', 'Category Three']);
        });
    });
});
