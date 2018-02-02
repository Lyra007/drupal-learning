<?php

/**
 * @param $form
 * @param $form_state
 *
 * @return mixed
 *
 * a custom form for ajax use
 *
 */
function modal_test_form($form, &$form_state){

  //  to store it first
  if($form_state['values']['nid'])
  {
    $form_state['nid'] = $form_state['values']['nid'];
  }

  //use tree to build different field with same values
  $form['#tree'] = TRUE;


  //set a wrapper to deal with the fieldset using ajsx
  $form['info_fieldset'] = array(
    '#type' => 'fieldset',
    '#title' => t('报名者信息'),
    '#prefix' => '<div id="info-fieldset-wrapper">',
    '#suffix' => '</div>',
  );

  // num_users is the num of textfield
  if(empty($form_state['num_users'])){
    $form_state['num_users'] = 1;
  }

  for($i = 0; $i < $form_state['num_users']; $i++)
  {

    $form['info_fieldset'][$i] = array(
      '#type' => 'fieldset'
    );

    $form['info_fieldset'][$i]['name'] = array(
      '#type' => 'textfield',
      '#title' => t('姓名'),
//      '#required' => TRUE,
    );

    $form['info_fieldset'][$i]['mobile'] = array(
      '#type' => 'textfield',
      '#title' => t('电话'),
//      '#required' => TRUE,
    );

    $form['info_fieldset'][$i]['nid'] = array(
      '#type' => 'hidden',
      '#value' => $form_state['nid']
    );
  }

  $form['info_fieldset']['add_row'] = array(
    '#type' => 'submit',
    '#value' => t('增加一组'),
    '#limit_validation_errors' => array(),       // No validation.
    '#submit' => array('add_row_add_one'),
    '#ajax' => array(
      'callback' => 'addone_callback',
      'wrapper' => 'info-fieldset-wrapper',
    ),
  );

  if ($form_state['num_users'] > 1)
  {
    $form['info_fieldset']['remove_row'] = array(
      '#type' => 'submit',
      '#value' => t('移除一组'),
      '#limit_validation_errors' => array(),       // No validation.
      '#submit' => array('remove_row_remove_one'),
      '#ajax' => array(
        'callback' => 'addone_callback',
        'wrapper' => 'info-fieldset-wrapper',
      ),
    );
  }


  $form['submit_button'] = array(
    '#type' => 'submit',
    '#value' => t('提交报名!'),
  );

  return $form;
}

/**
 * check whether it is a valid phone number
 */

function is_valid_phone($mobile){
  //check for phone validation
  //ranges are 131,132,133,135,136,139,151,152,153,156,158,159,188

  preg_match("/13[123569]{1}\d{8}|15[1235689]\d{8}|188\d{8}/", $mobile, $array);

  //  var_dump($array);
  if(count($array))
  {
    return TRUE;
  }
  return FALSE;
}


/**
 * check wthether the number is in db
 */

function number_is_in_db($mobile, $nid){

  $query = new EntityFieldQuery();

  $query -> entityCondition('entity_type', 'node')
    ->entityCondition('bundle', 'sighupsheet')
    ->propertyCondition('status', NODE_PUBLISHED)
    ->fieldCondition('field_mobile', 'value', $mobile, '=')
    ->fieldCondition('field_activity', 'target_id', $nid, '=')
    ->count();

  return $query->execute() > 0 ? TRUE : FALSE;
}





/**
 * @param $form
 * @param $form_state
 *
 * a form validation
 * including 3 different validation
 * 1. check if phone is valid
 * 2. if there is any same mobile phone in the form
 * 3. if there is any same mobile phone already exits in database
 */

function modal_test_form_validate($form, &$form_state) {



  $arr = $form_state['values']['info_fieldset'];
  $whole = array();

  for($i = 0; $arr[$i]!= NULL; $i++) {
    $name = $arr[$i]['name'];
    $mobile = $arr[$i]['mobile'];
    $nid = $arr[$i]['nid'];

    //check if name is set
    if(empty($name)){
      form_set_error('name', t('姓名不为空'));
    }

    if(empty($mobile)){
      form_set_error('mobile', t('手机号不为空'));
    }
    $row_num = $i + 1;

    //check if it is in whole array
    if(!isset($whole[$mobile])) {
      $whole[$mobile] = $i+1;
      //check for phone
      if (strlen($mobile) != 11) {
        form_set_error('mobile', t('第'.$row_num.'组手机号长度必须是十一位'));
      }
      if (!is_valid_phone($mobile)) {
        form_set_error('mobile', t('第'.$row_num.'组中手机号码无效, 必须在此范围内（131,132,133,135,136,139,151,152,153,156,158,159,188）'));
      }
      if (number_is_in_db($mobile, $nid)) {
        form_set_error('mobile', t('第'.$row_num.'组中有被使用的手机号'));
      }
    }else{
      $pre = $whole[$mobile];

      form_set_error('mobile', t('第'.$pre.'和第'.$row_num.'组中有重复的手机号'));
    }
  }
}


/**
 * @param $form
 * @param $form_state
 *
 * a form submition function
 * using a loop to submit form
 */
function modal_test_form_submit($form, &$form_state) {
  global $user;

  $arr = $form_state['values']['info_fieldset'];

  for($i = 0; $arr[$i]!= NULL; $i++){
    $node = entity_create('node', array('type' => 'sighupsheet'));
    $node->uid = $user->uid;
    $wrapper = entity_metadata_wrapper('node', $node);
    $wrapper->title ='dodo by ';
    $wrapper->field_name = $arr[$i]['name'];
    $wrapper->field_mobile = $arr[$i]['mobile'];
    $wrapper->field_activity = $arr[$i]['nid'];
    $wrapper->save();
  }

}

/**
 * @param $form
 * @param $form_state
 * a callback to add one row
 */
function add_row_add_one($form, &$form_state) {
  $form_state['num_users']++;
  $form_state['rebuild'] = TRUE;
}

/**
 * @param $form
 * @param $form_state
 * a callback to remove one row
 */
function remove_row_remove_one($form, &$form_state) {
  if ($form_state['num_users'] > 1) {
    $form_state['num_users']--;
  }
  $form_state['rebuild'] = TRUE;
}

/**
 * @param $form
 * @param $form_state
 *
 * @return mixed
 * an ajax callback function
 */

function addone_callback($form, $form_state) {
  return $form['info_fieldset'];
}




