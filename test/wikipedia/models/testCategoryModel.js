const CategoryModel = imports.models.category_model;

let mockJsonData = {
    category_name: 'Category Name',
    content_text: 'Lorem Ipsum',
    image_file: 'file:///image.jpg',
    image_thumb_uri: 'file:///thumb.jpg',
    is_main_category: false,
    subcategories: [ 'Category Two' ]
};

function testNewModelFromJson() {
    let model = CategoryModel.newFromJson(mockJsonData);
    assertTrue(model instanceof CategoryModel.CategoryModel);
    assertEquals('Category Name', model.id);
    assertEquals(0, model.getSubcategories().length);
}

function testNewWithProperties() {
    let model = new CategoryModel.CategoryModel({
        id: 'id',
        title: 'title',
        description: 'description',
        image_uri: 'image-uri',
        image_thumbnail_uri: 'image-thumbnail-uri',
        is_main_category: true,
        has_articles: true
    });
    assertEquals('id', model.id);
    assertEquals('title', model.title);
    assertEquals('description', model.description);
    assertEquals('image-uri', model.image_uri);
    assertEquals('image-thumbnail-uri', model.image_thumbnail_uri);
    assertEquals(true, model.is_main_category);
    assertEquals(true, model.has_articles);

    model.has_articles = false;
    assertEquals(false, model.has_articles);
}

function testGetSubcategoriesEmpty() {
    let model = new CategoryModel.CategoryModel();
    assertEquals(0, model.getSubcategories().length);
}

function _assertCategoryListContainsCategoryIds(categoryList, idList) {
    assertEquals(idList.length, categoryList.length);
    idList.forEach(function (id) {
        assertTrue(categoryList.some(function (categoryModel) {
            return categoryModel.id == id;
        }));
    });
}

function testAddAndGetSubcategories() {
    let model1 = new CategoryModel.CategoryModel({ id: 'Category One' });
    let model2 = new CategoryModel.CategoryModel({ id: 'Category Two' });
    let model3 = new CategoryModel.CategoryModel({ id: 'Category Three' });
    model1.addSubcategory(model2);
    model1.addSubcategory(model3);

    let categories = model1.getSubcategories();
    _assertCategoryListContainsCategoryIds(categories,
        ['Category Two', 'Category Three']);
}

function testAddSubcategoryDoesNothingForDuplicate() {
    let model1 = new CategoryModel.CategoryModel({ id: 'Category One' });
    let model2 = new CategoryModel.CategoryModel({ id: 'Category Two' });
    let model3 = new CategoryModel.CategoryModel({ id: 'Category Two' });
    model1.addSubcategory(model2);
    model1.addSubcategory(model3);

    let categories = model1.getSubcategories();
    _assertCategoryListContainsCategoryIds(categories, ['Category Two']);
}
