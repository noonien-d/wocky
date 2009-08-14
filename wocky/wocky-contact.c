/*
 * wocky-contact.c - Source for WockyContact
 * Copyright (C) 2009 Collabora Ltd.
 * @author Jonny Lamb <jonny.lamb@collabora.co.uk>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

/**
 * SECTION: wocky-contact
 * @title: WockyContact
 * @short_description: Wrapper around a roster item.
 * @include: wocky/wocky-contact.h
 *
 * Stores information regarding a roster item and provides a higher level API
 * for altering its details.
 */

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#include <gio/gio.h>

#include "wocky-contact.h"
#include "wocky-signals-marshal.h"
#include "wocky-utils.h"

#define DEBUG_FLAG DEBUG_ROSTER
#include "wocky-debug.h"

G_DEFINE_TYPE (WockyContact, wocky_contact, G_TYPE_OBJECT)

/* properties */
enum
{
  PROP_JID = 1,
  PROP_NAME,
  PROP_SUBSCRIPTION,
  PROP_GROUPS,
};

/* signal enum */
enum
{
  LAST_SIGNAL,
};

/*
static guint signals[LAST_SIGNAL] = {0};
*/

/* private structure */
typedef struct _WockyContactPrivate WockyContactPrivate;

struct _WockyContactPrivate
{
  gboolean dispose_has_run;

  gchar *jid;
  gchar *name;
  WockyRosterSubscriptionFlags subscription;
  gchar **groups;
};

#define WOCKY_CONTACT_GET_PRIVATE(o)  \
    (G_TYPE_INSTANCE_GET_PRIVATE ((o), WOCKY_TYPE_CONTACT, \
    WockyContactPrivate))

static void
wocky_contact_init (WockyContact *obj)
{
  /*
  WockyContact *self = WOCKY_CONTACT (obj);
  WockyContactPrivate *priv = WOCKY_CONTACT_GET_PRIVATE (self);
  */
}

static void
wocky_contact_set_property (GObject *object,
    guint property_id,
    const GValue *value,
    GParamSpec *pspec)
{
  WockyContactPrivate *priv =
      WOCKY_CONTACT_GET_PRIVATE (object);

  switch (property_id)
    {
    case PROP_JID:
      priv->jid = g_value_dup_string (value);
      break;
    case PROP_NAME:
      wocky_contact_set_name (WOCKY_CONTACT (object),
          g_value_get_string (value));
      break;
    case PROP_SUBSCRIPTION:
      priv->subscription = g_value_get_uint (value);
      break;
    case PROP_GROUPS:
      priv->groups = g_value_dup_boxed (value);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
      break;
    }
}

static void
wocky_contact_get_property (GObject *object,
    guint property_id,
    GValue *value,
    GParamSpec *pspec)
{
  WockyContactPrivate *priv =
      WOCKY_CONTACT_GET_PRIVATE (object);

  switch (property_id)
    {
    case PROP_JID:
      g_value_set_string (value, priv->jid);
      break;
    case PROP_NAME:
      g_value_set_string (value, priv->name);
      break;
    case PROP_SUBSCRIPTION:
      g_value_set_uint (value, priv->subscription);
      break;
    case PROP_GROUPS:
      g_value_set_boxed (value, priv->groups);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
      break;
    }
}

static void
wocky_contact_constructed (GObject *object)
{
  /*
  WockyContact *self = WOCKY_CONTACT (object);
  WockyContactPrivate *priv = WOCKY_CONTACT_GET_PRIVATE (self);
  */
}

static void
wocky_contact_dispose (GObject *object)
{
  WockyContact *self = WOCKY_CONTACT (object);
  WockyContactPrivate *priv = WOCKY_CONTACT_GET_PRIVATE (self);

  if (priv->dispose_has_run)
    return;

  priv->dispose_has_run = TRUE;

  if (G_OBJECT_CLASS (wocky_contact_parent_class)->dispose)
    G_OBJECT_CLASS (wocky_contact_parent_class)->dispose (object);
}

static void
wocky_contact_finalize (GObject *object)
{
  WockyContact *self = WOCKY_CONTACT (object);
  WockyContactPrivate *priv = WOCKY_CONTACT_GET_PRIVATE (self);

  if (priv->jid != NULL)
    g_free (priv->jid);

  if (priv->name != NULL)
    g_free (priv->name);

  if (priv->groups != NULL)
    g_strfreev (priv->groups);

  G_OBJECT_CLASS (wocky_contact_parent_class)->finalize (object);
}

static void
wocky_contact_class_init (WockyContactClass *wocky_contact_class)
{
  GObjectClass *object_class = G_OBJECT_CLASS (wocky_contact_class);
  GParamSpec *spec;

  g_type_class_add_private (wocky_contact_class,
      sizeof (WockyContactPrivate));

  object_class->constructed = wocky_contact_constructed;
  object_class->set_property = wocky_contact_set_property;
  object_class->get_property = wocky_contact_get_property;
  object_class->dispose = wocky_contact_dispose;
  object_class->finalize = wocky_contact_finalize;

  /**
   * WockyContact:jid:
   *
   * The contact's bare JID, according to the roster.
   */
  spec = g_param_spec_string ("jid", "Contact JID",
      "Contact JID",
      "",
      G_PARAM_READWRITE |
      G_PARAM_CONSTRUCT_ONLY | G_PARAM_STATIC_STRINGS);
  g_object_class_install_property (object_class, PROP_JID, spec);

  /**
   * WockyContact:name:
   *
   * The contact's name, according to the roster.
   */
  spec = g_param_spec_string ("name", "Contact Name",
      "Contact Name",
      "",
      G_PARAM_READWRITE |
      G_PARAM_CONSTRUCT | G_PARAM_STATIC_STRINGS);
  g_object_class_install_property (object_class, PROP_NAME, spec);

  /**
   * WockyContact:subscription:
   *
   * The subscription type of the contact, according to the roster.
   */
  spec = g_param_spec_uint ("subscription", "Contact Subscription",
      "Contact Subscription",
      0,
      LAST_WOCKY_ROSTER_SUBSCRIPTION_TYPE,
      WOCKY_ROSTER_SUBSCRIPTION_TYPE_NONE,
      G_PARAM_READWRITE |
      G_PARAM_CONSTRUCT_ONLY | G_PARAM_STATIC_STRINGS);
  g_object_class_install_property (object_class, PROP_SUBSCRIPTION, spec);

  /**
   * WockyContact:groups:
   *
   * A list of the contact's groups, according to the roster.
   */
  spec = g_param_spec_boxed ("groups", "Contact Groups",
      "Contact Groups",
      G_TYPE_STRV,
      G_PARAM_READWRITE |
      G_PARAM_CONSTRUCT_ONLY | G_PARAM_STATIC_STRINGS);
  g_object_class_install_property (object_class, PROP_GROUPS, spec);
}

/**
 * wocky_contact_get_jid:
 * @contact: a #WockyContact instance
 *
 * <!-- -->
 *
 * Returns: @contact's JID.
 */
const gchar *
wocky_contact_get_jid (WockyContact *contact)
{
  WockyContactPrivate *priv;

  g_return_val_if_fail (WOCKY_IS_CONTACT (contact), NULL);

  priv = WOCKY_CONTACT_GET_PRIVATE (contact);

  return priv->jid;
}

/**
 * wocky_contact_get_name:
 * @contact: #WockyContact instance
 *
 * <!-- -->
 *
 * Returns: @contact's name
 */
const gchar *
wocky_contact_get_name (WockyContact *contact)
{
  WockyContactPrivate *priv;

  g_return_val_if_fail (WOCKY_IS_CONTACT (contact), NULL);

  priv = WOCKY_CONTACT_GET_PRIVATE (contact);

  return priv->name;
}

/* FIXME: document that wocky_contact_set_* shouldn't be used by users */

/**
 * wocky_contact_set_name:
 * @contact: a #WockyContact instance
 * @name: the name to set @contact
 *
 * Sets @contact's name to @name.
 *
 */
void
wocky_contact_set_name (WockyContact *contact,
    const gchar *name)
{
  WockyContactPrivate *priv;

  g_return_if_fail (WOCKY_IS_CONTACT (contact));

  priv = WOCKY_CONTACT_GET_PRIVATE (contact);

  if (!wocky_strdiff (priv->name, name))
    return;

  g_free (priv->name);
  priv->name = g_strdup (name);
  g_object_notify (G_OBJECT (contact), "name");
}

/**
 * wocky_contact_get_subscription:
 * @contact: a #WockyContact instance
 *
 * <!-- -->
 *
 * Returns: @contact's subscription.
 */
WockyRosterSubscriptionFlags
wocky_contact_get_subscription (WockyContact *contact)
{
  WockyContactPrivate *priv;

  g_return_val_if_fail (WOCKY_IS_CONTACT (contact),
      WOCKY_ROSTER_SUBSCRIPTION_TYPE_NONE);
  priv = WOCKY_CONTACT_GET_PRIVATE (contact);

  return priv->subscription;
}

/**
 * wocky_contact_set_subscription:
 * @contact: a #WockyContact instance
 * @subscription: the new subscription type
 *
 * Sets the subscription of @contact.
 *
 */
void
wocky_contact_set_subscription (WockyContact *contact,
    WockyRosterSubscriptionFlags subscription)
{
  WockyContactPrivate *priv;

  g_return_if_fail (WOCKY_IS_CONTACT (contact));

  priv = WOCKY_CONTACT_GET_PRIVATE (contact);

  if (priv->subscription == subscription)
    return;

  priv->subscription = subscription;
  g_object_notify (G_OBJECT (contact), "subscription");
}

/**
 * wocky_contact_get_groups:
 * @contact: a #WockyContact instance
 *
 * <!-- -->
 *
 * Returns: a list of @contact's groups
 */
const gchar * const *
wocky_contact_get_groups (WockyContact *contact)
{
  WockyContactPrivate *priv;

  g_return_val_if_fail (WOCKY_IS_CONTACT (contact), NULL);
  priv = WOCKY_CONTACT_GET_PRIVATE (contact);

  return (const gchar * const *) priv->groups;
}

static gint
cmp_str (gchar **a,
    gchar **b)
{
  return strcmp (*a, *b);
}

static GPtrArray *
sort_groups (GStrv groups)
{
  GPtrArray *arr;
  guint i;

  arr = g_ptr_array_sized_new (g_strv_length (groups));
  for (i = 0; groups[i] != NULL; i++)
    {
      g_ptr_array_add (arr, groups[i]);
    }

  g_ptr_array_sort (arr, (GCompareFunc) cmp_str);

  return arr;
}

static gboolean
groups_equal (const gchar * const * groups_a,
  const gchar * const * groups_b)
{
  GPtrArray *arr_a, *arr_b;
  guint i;
  gboolean result = TRUE;

  if (groups_a == NULL && groups_b == NULL)
    return TRUE;

  if (groups_a == NULL || groups_b == NULL)
    return FALSE;

  if (g_strv_length ((GStrv) groups_a) != g_strv_length ((GStrv) groups_b))
    return FALSE;

  /* Sort both groups array and then compare elements one by one */
  arr_a = sort_groups ((GStrv) groups_a);
  arr_b = sort_groups ((GStrv) groups_b);

  for (i = 0; i != arr_a->len && result; i++)
    {
      const gchar *a = g_ptr_array_index (arr_a, i);
      const gchar *b = g_ptr_array_index (arr_b, i);

      if (wocky_strdiff (a, b))
        result = FALSE;
    }

  g_ptr_array_free (arr_a, TRUE);
  g_ptr_array_free (arr_b, TRUE);
  return result;
}

/**
 * wocky_contact_set_groups:
 * @contact: a #WockyContact instance
 * @groups: a list of groups
 *
 * Sets @contact's groups.
 *
 */
void
wocky_contact_set_groups (WockyContact *contact,
    gchar **groups)
{
  WockyContactPrivate *priv;

  g_return_if_fail (WOCKY_IS_CONTACT (contact));

  priv = WOCKY_CONTACT_GET_PRIVATE (contact);

  if (groups_equal ((const gchar * const *) groups,
        (const gchar * const *) priv->groups))
    return;

  if (priv->groups != NULL)
    g_strfreev (priv->groups);

  priv->groups = g_strdupv (groups);
  g_object_notify (G_OBJECT (contact), "groups");
}

gboolean
wocky_contact_equal (WockyContact *a,
    WockyContact *b)
{
  const gchar * const * groups_a;
  const gchar * const * groups_b;

  g_assert (a != NULL || b != NULL);

  if (a == NULL || b == NULL)
    return FALSE;

  if (wocky_strdiff (wocky_contact_get_jid (a), wocky_contact_get_jid (b)))
    return FALSE;

  if (wocky_strdiff (wocky_contact_get_name (a), wocky_contact_get_name (b)))
    return FALSE;

  if (wocky_contact_get_subscription (a) != wocky_contact_get_subscription (b))
    return FALSE;

  groups_a = wocky_contact_get_groups (a);
  groups_b = wocky_contact_get_groups (b);

  return groups_equal (groups_a, groups_b);
}

void
wocky_contact_add_group (WockyContact *self,
    const gchar *group)
{
  WockyContactPrivate *priv = WOCKY_CONTACT_GET_PRIVATE (self);
  GPtrArray *arr;
  gboolean group_already_present = FALSE;

  if (priv->groups != NULL)
    {
      guint len, i;

      len = g_strv_length (priv->groups);
      arr = g_ptr_array_sized_new (len + 2);

      for (i = 0; priv->groups[i] != NULL; i++)
        {
          g_ptr_array_add (arr, g_strdup (priv->groups[i]));

          if (!wocky_strdiff (priv->groups[i], group))
            /* Don't add the group twice */
            group_already_present = TRUE;
        }

      g_strfreev (priv->groups);
    }
  else
    {
      arr = g_ptr_array_sized_new (2);
    }

  if (!group_already_present)
    g_ptr_array_add (arr, g_strdup (group));

  /* Add trailing NULL */
  g_ptr_array_add (arr, NULL);

  priv->groups = (GStrv) g_ptr_array_free (arr, FALSE);
}

gboolean
wocky_contact_in_group (WockyContact *self,
    const gchar *group)
{
  WockyContactPrivate *priv = WOCKY_CONTACT_GET_PRIVATE (self);
  guint i;

  if (priv->groups == NULL)
    return FALSE;

  for (i = 0; priv->groups[i] != NULL; i++)
    {
      if (!wocky_strdiff (priv->groups[i], group))
        return TRUE;
    }

  return FALSE;
}
