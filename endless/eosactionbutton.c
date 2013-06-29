/* Copyright 2013 Endless Mobile, Inc. */

#include "config.h"
#include "eosactionbutton.h"

#include <glib-object.h>
#include <gtk/gtk.h>
#include <math.h>

/**
 * SECTION:action-button
 * @short_description: Buttons that the user recognizes as performing actions
 * @title: Action buttons
 *
 * Any time you want to inform your user which actions are available, use an
 * action button.
 * For example, suppose you had a page in your application where the user could
 * draw a picture.
 * After finishing the picture, the user could save it or share it on Facebook.
 * In that case, you would use two action buttons, labeled for example
 * <quote>SAVE</quote> and <quote>SHARE</quote>, and containing icons
 * representing saving and sharing.
 *
 * The buttons have a recognizable style and round border, so that it is
 * instantly clear to the user that they represent actions.
 *
 * Generally, you should set the #EosPageManager:actions property on the page
 * that you want to use action buttons on; this neatly arranges the actions
 * in an action area on the right-hand side of the screen, placing the main
 * action in a prominent place, and actions such as <quote>cancel</quote> on the
 * bottom.
 * However, you can also manually place action buttons anywhere on a page.
 */

#define _EOS_STYLE_CLASS_ACTION_BUTTON "action-button"

G_DEFINE_TYPE (EosActionButton, eos_action_button, GTK_TYPE_BUTTON)

#define EOS_ACTION_BUTTON_PRIVATE(o) \
  (G_TYPE_INSTANCE_GET_PRIVATE ((o), EOS_TYPE_ACTION_BUTTON, EosActionButtonPrivate))

struct _EosActionButtonPrivate
{
  /* properties */
  EosActionButtonSize size;
  gchar *label;
  gchar *icon_id;

  /* internal */
  GtkWidget *grid;
  GtkWidget *icon_image;
  GdkPixbuf *icon_pixbuf;
  GtkWidget *label_widget;
};

typedef struct _EosActionButtonSizeDefinition EosActionButtonSizeDefinition;

struct _EosActionButtonSizeDefinition
{
  EosActionButtonSize size;
  gchar *name;

  gint width;
  gint height;
  gint icon_size;
  gint border_width;
};

static EosActionButtonSizeDefinition *icon_sizes = NULL;


enum {
  PROP_0,
  PROP_SIZE,
  PROP_LABEL,
  PROP_ICON_ID
};

static void
eos_action_button_dispose (GObject *object);

static void
eos_action_button_finalize (GObject *object);

static void
eos_action_button_get_property (GObject    *object,
                                guint       property_id,
                                GValue     *value,
                                GParamSpec *pspec);

static void
eos_action_button_set_property (GObject      *object,
                                guint         property_id,
                                const GValue *value,
                                GParamSpec   *pspec);

static void
eos_action_button_get_preferred_width (GtkWidget *widget,
                                       gint      *minimum_size,
                                       gint      *natural_size);

static void
eos_action_button_get_preferred_height (GtkWidget *widget,
                                        gint      *minimum_size,
                                        gint      *natural_size);

static gboolean
eos_action_button_draw (GtkWidget *widget,
                        cairo_t   *cr);

/* ******* INIT ******* */

static void
eos_action_button_class_init (EosActionButtonClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);
  GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (klass);

  g_type_class_add_private (klass, sizeof (EosActionButtonPrivate));

  object_class->get_property = eos_action_button_get_property;
  object_class->set_property = eos_action_button_set_property;
  object_class->dispose = eos_action_button_dispose;
  object_class->finalize = eos_action_button_finalize;

  widget_class->draw = eos_action_button_draw;
  widget_class->get_preferred_width  = eos_action_button_get_preferred_width;
  widget_class->get_preferred_height = eos_action_button_get_preferred_height;

  /**
   * EosActionButton:size:
   *
   * Size for the action button; use #EosActionButtonSize to specify that the
   * button represents a primary action, secondary, tertiary, or quaternary.
   * #EOS_ACTION_BUTTON_SIZE_PRIMARY is the largest, and
   * #EOS_ACTION_BUTTON_SIZE_QUATERNARY is the smallest (seldom used.)
   */
  g_object_class_install_property (object_class,
                                   PROP_SIZE,
                                   g_param_spec_int ("size",
                                                     "Size",
                                                     "Size of the button",
                                                     EOS_ACTION_BUTTON_SIZE_PRIMARY,
                                                     EOS_ACTION_BUTTON_SIZE_QUATERNARY,
                                                     EOS_ACTION_BUTTON_SIZE_SECONDARY,
                                                     G_PARAM_READWRITE | G_PARAM_CONSTRUCT));

  /**
   * EosActionButton:label:
   *
   * Text for the label that is placed below or to the side of the button.
   */
  g_object_class_install_property (object_class,
                                   PROP_LABEL,
                                   g_param_spec_string ("label",
                                                        "Label",
                                                        "Text of the label widget beneath the button",
                                                        NULL,
                                                        G_PARAM_READWRITE | G_PARAM_CONSTRUCT));

  /**
   * EosActionButton:icon-id:
   *
   * Icon name for the icon that is drawn within the circular button.
   */
  g_object_class_install_property (object_class,
                                   PROP_ICON_ID,
                                   g_param_spec_string ("icon-id",
                                                        "Icon id",
                                                        "ID used to pick an icon for the button",
                                                        NULL,
                                                        G_PARAM_READWRITE | G_PARAM_CONSTRUCT));

  // init icon sizes, inspired by gtkiconfactory.c
  if (icon_sizes == NULL)
    {
      icon_sizes = g_new (EosActionButtonSizeDefinition, EOS_ACTION_BUTTON_SIZE_NUM_SIZES);

      icon_sizes[EOS_ACTION_BUTTON_SIZE_PRIMARY].size = EOS_ACTION_BUTTON_SIZE_PRIMARY;
      icon_sizes[EOS_ACTION_BUTTON_SIZE_PRIMARY].name = "primary";
      icon_sizes[EOS_ACTION_BUTTON_SIZE_PRIMARY].width = 64;
      icon_sizes[EOS_ACTION_BUTTON_SIZE_PRIMARY].height = 64;
      icon_sizes[EOS_ACTION_BUTTON_SIZE_PRIMARY].icon_size = 36;
      icon_sizes[EOS_ACTION_BUTTON_SIZE_PRIMARY].border_width = 8;

      icon_sizes[EOS_ACTION_BUTTON_SIZE_SECONDARY].size = EOS_ACTION_BUTTON_SIZE_SECONDARY;
      icon_sizes[EOS_ACTION_BUTTON_SIZE_SECONDARY].name = "secondary";
      icon_sizes[EOS_ACTION_BUTTON_SIZE_SECONDARY].width = 48;
      icon_sizes[EOS_ACTION_BUTTON_SIZE_SECONDARY].height = 48;
      icon_sizes[EOS_ACTION_BUTTON_SIZE_SECONDARY].icon_size = 26;
      icon_sizes[EOS_ACTION_BUTTON_SIZE_SECONDARY].border_width = 6;

      icon_sizes[EOS_ACTION_BUTTON_SIZE_TERTIARY].size = EOS_ACTION_BUTTON_SIZE_TERTIARY;
      icon_sizes[EOS_ACTION_BUTTON_SIZE_TERTIARY].name = "tertiary";
      icon_sizes[EOS_ACTION_BUTTON_SIZE_TERTIARY].width = 36;
      icon_sizes[EOS_ACTION_BUTTON_SIZE_TERTIARY].height = 36;
      icon_sizes[EOS_ACTION_BUTTON_SIZE_TERTIARY].icon_size = 18;
      icon_sizes[EOS_ACTION_BUTTON_SIZE_TERTIARY].border_width = 5;

      icon_sizes[EOS_ACTION_BUTTON_SIZE_QUATERNARY].size = EOS_ACTION_BUTTON_SIZE_QUATERNARY;
      icon_sizes[EOS_ACTION_BUTTON_SIZE_QUATERNARY].name = "quaternary";
      icon_sizes[EOS_ACTION_BUTTON_SIZE_QUATERNARY].width = 26;
      icon_sizes[EOS_ACTION_BUTTON_SIZE_QUATERNARY].height = 26;
      icon_sizes[EOS_ACTION_BUTTON_SIZE_QUATERNARY].icon_size = 12;
      icon_sizes[EOS_ACTION_BUTTON_SIZE_QUATERNARY].border_width = 4;
    }
}

static void
eos_action_button_init (EosActionButton *self)
{
  EosActionButtonPrivate *priv;
  GtkStyleContext *context;

  self->priv = EOS_ACTION_BUTTON_PRIVATE (self);
  priv = self->priv;

  context = gtk_widget_get_style_context (GTK_WIDGET (self));
  gtk_style_context_add_class (context, _EOS_STYLE_CLASS_ACTION_BUTTON);

  priv->icon_image = gtk_image_new();
  priv->label_widget = GTK_WIDGET (gtk_label_new (""));

  priv->grid = gtk_grid_new ();
  gtk_grid_attach(GTK_GRID (priv->grid), priv->icon_image, 0, 0, 1, 1);
  gtk_grid_attach(GTK_GRID (priv->grid), priv->label_widget, 0, 1, 1, 1);

  gtk_container_add (GTK_CONTAINER (self),
                     GTK_WIDGET (priv->grid));

  // TODO positioning is not really working right, it will be done manually in draw ()

  gtk_widget_set_hexpand (GTK_WIDGET(self), FALSE);
  gtk_widget_set_halign (GTK_WIDGET(self), GTK_ALIGN_CENTER);
  gtk_widget_set_vexpand (GTK_WIDGET(self), FALSE);
  gtk_widget_set_valign (GTK_WIDGET(self), GTK_ALIGN_CENTER);
}

/* ******* LIFECYCLE ******* */

/**
 * eos_action_button_new:
 * @size: size for the button: primary, secondary, et cetera
 * @label: text to place under or beside the action button
 * @icon_id: icon name for the icon inside the action button
 *
 * Convenience function for creating an action button with the size, label, and
 * icon already set.
 *
 * Returns: a pointer to the newly-created widget.
 */
GtkWidget *
eos_action_button_new (EosActionButtonSize size,
                       const gchar *label,
                       const gchar *icon_id)
{
  return g_object_new (EOS_TYPE_ACTION_BUTTON,
                       "size", size,
                       "label", label,
                       "icon-id", icon_id,
                       NULL);
}

static void
eos_action_button_dispose (GObject *object)
{
  G_OBJECT_CLASS (eos_action_button_parent_class)->dispose (object);
}

static void
eos_action_button_finalize (GObject *object)
{
  G_OBJECT_CLASS (eos_action_button_parent_class)->finalize (object);
}

/* ******* PROPERTIES ******* */

static void
eos_action_button_load_icon (EosActionButton *button)
{
  EosActionButtonPrivate *priv;
  GtkIconInfo *icon_info;
  GdkPixbuf *new_icon = NULL;
  gboolean was_symbolic = TRUE;
  GError *error = NULL;

  g_return_if_fail (EOS_IS_ACTION_BUTTON (button));

  priv = button->priv;

  // TODO maybe use gtk_image_set_from_icon_set

  if (priv->icon_id != NULL)
    {
      icon_info = gtk_icon_theme_lookup_icon (gtk_icon_theme_get_default (),
                                              priv->icon_id,
                                              icon_sizes[priv->size].icon_size,
                                              GTK_ICON_LOOKUP_FORCE_SIZE
                                              | GTK_ICON_LOOKUP_GENERIC_FALLBACK
                                              | GTK_ICON_LOOKUP_USE_BUILTIN );

      new_icon = gtk_icon_info_load_symbolic_for_context (icon_info,
                                                          gtk_widget_get_style_context (GTK_WIDGET(button)),
                                                          &was_symbolic,
                                                          &error);

      if (!was_symbolic)
        {
          g_warning ("Icon for %s is not symbolic\n", priv->icon_id);
        }
      if (error != NULL)
        {
          g_warning ("Unable to load icon for %s : %s\n", priv->icon_id, error->message);
          g_error_free (error);
        }
      g_object_ref (new_icon);
      g_object_unref (icon_info);
    }
  else
    {
      new_icon = NULL;
    }

  if (priv->icon_pixbuf != NULL)
    {
      g_object_unref (priv->icon_pixbuf);
    }

  priv->icon_pixbuf = new_icon;

  gtk_image_set_from_pixbuf (GTK_IMAGE (priv->icon_image), priv->icon_pixbuf);
}

/**
 * eos_action_button_set_size:
 * @button: the button
 * @size: a value from #EosActionButtonSize
 *
 * Sets the size of the button (e.g. secondary).
 * See #EosActionButton:size for more information.
 */
void
eos_action_button_set_size  (EosActionButton *button,
                             EosActionButtonSize size)
{
  EosActionButtonPrivate *priv;
  int old_size;

  g_return_if_fail (EOS_IS_ACTION_BUTTON (button));

  priv = button->priv;

  old_size = priv->size;
  priv->size = size;

  // remove the old style class and set the new one
  GtkStyleContext *context = gtk_widget_get_style_context (GTK_WIDGET (button));
  gtk_style_context_remove_class (context, icon_sizes[old_size].name);
  gtk_style_context_add_class (context, icon_sizes[priv->size].name);

  if (old_size != priv->size)
    {
      eos_action_button_load_icon (button);

      g_object_notify (G_OBJECT (button), "size");
      gtk_widget_queue_resize(GTK_WIDGET (button));
    }
}

/**
 * eos_action_button_get_size:
 * @button: the button
 *
 * Gets the size of the button (e.g. secondary).
 * See #EosActionButton:size for more information.
 *
 * Returns: the size as a value from #EosActionButtonSize
 */
EosActionButtonSize
eos_action_button_get_size (EosActionButton *button)
{
  EosActionButtonPrivate *priv;

  g_return_val_if_fail (EOS_IS_ACTION_BUTTON (button), -1);

  priv = button->priv;

  return priv->size;
}

/**
 * eos_action_button_set_label:
 * @button: the button
 * @label: text for the label
 *
 * Sets the text showing below or beside the button.
 * See #EosActionButton:label for more information.
 */
void
eos_action_button_set_label (EosActionButton *button, const gchar *label)
{
  EosActionButtonPrivate *priv;
  gchar *new_label;

  g_return_if_fail (EOS_IS_ACTION_BUTTON (button));

  priv = button->priv;
  new_label = g_strdup (label);
  g_free (priv->label);
  priv->label = new_label;

  gtk_label_set_text (GTK_LABEL (priv->label_widget), priv->label);

  g_object_notify (G_OBJECT (button), "label");
}

/**
 * eos_action_button_get_label:
 * @button: the button
 *
 * Gets the text showing below or beside the button.
 * See #EosActionButton:label for more information.
 *
 * Returns: the label text
 */
const gchar *
eos_action_button_get_label (EosActionButton *button)
{
  EosActionButtonPrivate *priv;

  g_return_val_if_fail (EOS_IS_ACTION_BUTTON (button), NULL);

  priv = button->priv;

  return priv->label;
}

/**
 * eos_action_button_set_icon_id:
 * @button: the button
 * @icon_id: an icon name
 *
 * Sets a new icon showing in the button, specified by icon name.
 * See #EosActionButton:icon-id for more information.
 */
void
eos_action_button_set_icon_id (EosActionButton *button,
                               const gchar* icon_id)
{
  EosActionButtonPrivate *priv;

  g_return_if_fail (EOS_IS_ACTION_BUTTON (button));
  priv = button->priv;

  if (g_strcmp0 (icon_id, priv->icon_id) != 0)
    {
      g_free (priv->icon_id);
      priv->icon_id = g_strdup (icon_id);

      eos_action_button_load_icon (button);
      g_object_notify (G_OBJECT (button), "icon-id");
    }
}

/**
 * eos_action_button_get_icon_id:
 * @button: the button
 *
 * Gets the icon name for the icon showing in the button.
 * See #EosActionButton:icon-id for more information.
 *
 * Returns: an icon name
 */
const gchar *
eos_action_button_get_icon_id (EosActionButton *button)
{
  g_return_val_if_fail (EOS_IS_ACTION_BUTTON (button), NULL);

  return button->priv->icon_id;
}

static void
eos_action_button_get_property (GObject    *object,
                                guint       property_id,
                                GValue     *value,
                                GParamSpec *pspec)
{
  EosActionButton *button = EOS_ACTION_BUTTON (object);
  EosActionButtonPrivate *priv = button->priv;

  switch (property_id)
  {
  case PROP_SIZE:
    g_value_set_int (value, priv->size);
    break;
  case PROP_LABEL:
    g_value_set_string (value, priv->label);
    break;
  case PROP_ICON_ID:
    g_value_set_string (value, priv->icon_id);
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
  }
}

static void
eos_action_button_set_property (GObject      *object,
                                guint         property_id,
                                const GValue *value,
                                GParamSpec   *pspec)
{
  EosActionButton *button = EOS_ACTION_BUTTON (object);

  switch (property_id)
  {
  case PROP_SIZE :
    eos_action_button_set_size (button, g_value_get_int (value));
    break;
  case PROP_LABEL :
    eos_action_button_set_label (button, g_value_get_string (value));
    break;
  case PROP_ICON_ID :
    eos_action_button_set_icon_id (button, g_value_get_string (value));
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
  }
}

/* ******* EXTENDED METHODS ******* */

static void
eos_action_button_get_real_size (GtkWidget      *widget,
                                 GtkOrientation  orientation,
                                 gint           *minimum_size,
                                 gint           *natural_size)
{
  EosActionButton *button = EOS_ACTION_BUTTON (widget);
  EosActionButtonPrivate *priv = button->priv;
  GtkBorder margin;
  GtkAllocation label_allocation;
  GtkStyleContext *context = gtk_widget_get_style_context (widget);

  gtk_style_context_get_margin(context,
                               gtk_style_context_get_state (context),
                               &margin);

  gtk_widget_get_allocation (priv->label_widget, &label_allocation);

  if (minimum_size && orientation == GTK_ORIENTATION_HORIZONTAL)
    *minimum_size = MAX (icon_sizes[priv->size].width + margin.left + margin.right,
                         label_allocation.width);

  if (minimum_size && orientation == GTK_ORIENTATION_VERTICAL)
    *minimum_size = margin.top + icon_sizes[priv->size].height + margin.bottom +
                      label_allocation.height + margin.bottom;

  if (natural_size && orientation == GTK_ORIENTATION_HORIZONTAL)
    *natural_size = MAX (icon_sizes[priv->size].width + margin.left + margin.right,
                         label_allocation.width);

  if (natural_size && orientation == GTK_ORIENTATION_VERTICAL)
    *natural_size = margin.top + icon_sizes[priv->size].height + margin.bottom +
                      label_allocation.height + margin.bottom;
}

static void
eos_action_button_get_preferred_width (GtkWidget *widget,
                                       gint      *minimum_size,
                                       gint      *natural_size)
{
  eos_action_button_get_real_size (widget, GTK_ORIENTATION_HORIZONTAL,
                                   minimum_size, natural_size);
}

static void
eos_action_button_get_preferred_height (GtkWidget *widget,
                                        gint      *minimum_size,
                                        gint      *natural_size)
{
  eos_action_button_get_real_size (widget, GTK_ORIENTATION_VERTICAL,
                                   minimum_size, natural_size);
}

static gboolean
eos_action_button_draw (GtkWidget *widget,
                        cairo_t   *cr)
{
  EosActionButton *button = EOS_ACTION_BUTTON (widget);
  EosActionButtonPrivate *priv = button->priv;
  gint x, y;
  gint focus_width;
  gint focus_pad;
  GtkAllocation allocation;
  GtkStyleContext *context;
  GtkStateFlags state;
  gint width, height, border_width, border_height;
  GtkBorder margin;

  context = gtk_widget_get_style_context (widget);
  state = gtk_style_context_get_state (context);

  gtk_style_context_get_style (context,
                               "focus-line-width", &focus_width,
                               "focus-padding", &focus_pad,
                               NULL);

  gtk_style_context_get_margin(context,
                               state,
                               &margin);

  gtk_widget_get_allocation (widget, &allocation);

  x = 0;
  y = 0;
  width = allocation.width;
  height = allocation.height;

  border_width = icon_sizes[priv->size].width;
  border_height = icon_sizes[priv->size].height;

  cairo_save (cr);

  gtk_render_frame (context, cr,
                    x + (width - border_width)/2,
                    margin.top,
                    border_width, border_height);

  if (gtk_widget_has_visible_focus (widget))
    {
      gtk_render_focus (context, cr,
                        x, y, width, height);
    }

  // TODO is it really needed to restore and save the cairo_t here?
  cairo_restore (cr);
  cairo_save (cr);

  // *** image

  gtk_widget_get_allocation (priv->icon_image, &allocation);
  cairo_translate (cr,
                   (width - allocation.width) / 2,
                   margin.top + (icon_sizes[priv->size].height - allocation.height) / 2);

  gtk_widget_draw (GTK_WIDGET (priv->icon_image), cr);

  // TODO same as previous
  cairo_restore (cr);
  cairo_save (cr);

  // *** label

  gtk_widget_get_allocation (priv->label_widget, &allocation);
  cairo_translate (cr, x + (width - allocation.width)/2,
                   margin.top + icon_sizes[priv->size].height + margin.bottom);

  gtk_widget_draw (GTK_WIDGET (priv->label_widget), cr);

  cairo_restore (cr);

  return FALSE;
}
