<?php

function modal_test_form($form, &$form_state){

  //  to store it first
  if($form_state['values']['nid']){
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

  for($i = 0; $i < $form_state['num_users']; $i++){

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
    '#submit' => array('add_row_add_one'),

    '#ajax' => array(
      'callback' => 'addone_callback',
      'wrapper' => 'info-fieldset-wrapper',
    ),
  );

  if ($form_state['num_users'] > 1) {
    $form['info_fieldset']['remove_row'] = array(
      '#type' => 'submit',
      '#value' => t('移除一组'),
      '#submit' => array('remove_row_remove_one'),
      '#ajax' => array(
        'callback' => 'addone_callback',
        'wrapper' => 'info-fieldset-wrapper',
      ),
    );
  }



  //  $form['name'] = array(
  //    '#type' => 'textfield',
  //    '#title' => 'name',
  //    '#required' => TRUE,
  //  );
  //
  //  $form['mobile'] = array(
  //    '#type' => 'textfield',
  //    '#title' => 'mobile',
  //    '#required' => TRUE,
  //  );
  //  $form['nid'] = array(
  //    '#type' => 'hidden',
  //    '#value' => $nid
  //  );
  //
  //  $form['add_button'] = array(
  //    '#type' => 'button',
  //    '#value' => t('Add More')
  //  );
  //
  $form['submit_button'] = array(
    '#type' => 'submit',
    '#value' => t('提交报名!'),
  );

  return $form;
}

function modal_test_form_validate($form, &$form_state) {
}

function modal_test_form_submit($form, &$form_state) {
  global $user;

  //  $nid = $form_state['values']['nid'];

  $arr = $form_state['values']['info_fieldset'];
  //  var_dump($form_state);
  //  exit();

  //var_dump($form_state['values']);
  //$num=count($arr);

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

  //
  //  $node = new stdClass();
  //  $node->type = "sighupsheet";
  //  $node->status = 1;
  //  $node->promote = 0;
  //  $node->title = $title;
  //
  //  $node->field_name['und'][0]['value'] = $name;
  //  $node->field_mobile['und'][0]['value'] = $mobile;
  //
  //  node_object_prepare($node);
  //  node_save($node);

}

function add_row_add_one($form, &$form_state) {
  $form_state['num_users']++;
  $form_state['rebuild'] = TRUE;
}

function remove_row_remove_one($form, &$form_state) {
  if ($form_state['num_users'] > 1) {
    $form_state['num_users']--;
  }
  $form_state['rebuild'] = TRUE;
}


function addone_callback($form, $form_state) {
  return $form['info_fieldset'];
}




