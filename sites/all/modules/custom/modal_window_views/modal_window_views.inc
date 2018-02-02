<?php

/**
 * @param $form
 * @param $form_state
 *
 * @return mixed
 * a simple sign up form layout
 */

function modal_window_views_form($form, &$form_state)
{

  $nid = $form_state['values']['nid'];

  //Load the node object
  $node = node_load($nid);
  $name = $node->field_name[LANGUAGE_NONE][0]['value'];
  $mobile = $node->field_mobile[LANGUAGE_NONE][0]['value'];


  //set original value to default
  $form['name'] = array(
    '#type' => 'textfield',
    '#title' => 'name',
    '#default_value' => $name,
  );

  $form['mobile'] = array(
    '#type' => 'textfield',
    '#title' => 'mobile',
    '#default_value' => $mobile,
  );

  $form['nid'] = array(
    '#type' => 'hidden',
    '#value' => $nid
  );

  $form['submit_button'] = array(
    '#type' => 'submit',
    '#value' => t('Click Here!'),
  );

  return $form;
}

/**
 * This function is used to judge
 * whether it is a valid phone number
 */

function is_valid_phone($mobile){
  //check for phone validation
  //ranges are 131,132,133,135,136,139,151,152,153,156,158,159,188

  preg_match("/13[123569]{1}\d{8}|15[1235689]\d{8}|188\d{8}/", $mobile, $array);

//  var_dump($array);
//  if(count($array))
//
//    return TRUE;
//  }
//    return FALSE;
//
  return count($array) > 0;
}


/**
 * This function is used to judge
 * wthether the number is in db
 */

function number_is_in_db($mobile, $nid){

 $query = new EntityFieldQuery();

 $query -> entityCondition('entity_type', 'node')
   ->entityCondition('bundle', 'sighupsheet')
   ->propertyCondition('status', NODE_PUBLISHED)
   ->fieldCondition('field_mobile', 'value', $mobile, '=')
   ->fieldCondition('field_activity', 'target_id', $nid, '=')
   ->count();


  return $query->execute() > 0 ;
}


/**
 * @param $form
 * @param $form_state
 * a form validation
 * two different ways to validate
 * func isvalidphone: one is to valicate whether it is a valid phone number
 * func the other is to make sure whether the phone number is already exist
 */
function modal_window_views_form_validate($form, &$form_state)
{
  $mobile = $form_state['values']['mobile'];
  $nid = $form_state['values']['nid'];

  //check for phone
  if(strlen($mobile) != 11 ){
    form_set_error('mobile', t('手机号长度必须是十一位'));
  }
  if(!is_valid_phone($mobile)){
    form_set_error('mobile', t('手机号码无效, 必须在此范围内（131,132,133,135,136,139,151,152,153,156,158,159,188）'));
  }
  if(number_is_in_db($mobile, $nid)){
    form_set_error('mobile', t('该手机号已经被使用'));
  }

}

/**
 * @param $form
 * @param $form_state
 * a form submition
 */
function modal_window_views_form_submit($form, &$form_state)
{
  $nid = $form_state['values']['nid'];
  $name = $form_state['values']['name'];
  $mobile = $form_state['values']['mobile'];


  $node = node_load($nid);
  $wrapper = entity_metadata_wrapper('node', $node);

  // modify the original value
  $wrapper->field_name->set($name);
  $wrapper->field_mobile->set($mobile);

  $wrapper->save();

}

