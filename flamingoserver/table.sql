//�û���Ϣ��
CREATE TABLE IF NOT EXISTS  t_user  (
         f_id  bigint(20) NOT NULL AUTO_INCREMENT COMMENT '����ID',
		 f_user_id  bigint(20) NOT NULL COMMENT '�û�ID',
		 f_username  varchar(64) NOT NULL COMMENT '�û���',
         f_nickname  varchar(64) NOT NULL COMMENT '�û��ǳ�',
         f_password  varchar(64) DEFAULT NULL COMMENT '�û�����',
		 f_facetype  int(10) DEFAULT 0 COMMENT '�û�ͷ������',
		 f_customface  varchar(32) DEFAULT NULL COMMENT '�Զ���ͷ����',
		 f_customfacefmt  varchar(6) DEFAULT NULL COMMENT '�Զ���ͷ���ʽ',
		 f_gender  int(2) DEFAULT 0 COMMENT '�Ա�',
		 f_birthday  bigint(20) DEFAULT 19900101 COMMENT '����',
		 f_signature  varchar(256) DEFAULT NULL COMMENT '����ǩ��',
		 f_address  varchar(256) DEFAULT NULL COMMENT '��ַ',
		 f_phonenumber  varchar(64) DEFAULT NULL COMMENT '�绰',
		 f_mail  varchar(256) DEFAULT NULL COMMENT '����',
         f_register_time  datetime NOT NULL COMMENT 'ע��ʱ��',
		 f_owner_id  bigint(20) DEFAULT 0 COMMENT 'Ⱥ�˺�Ⱥ��userid',
         f_remark  varchar(64) DEFAULT NULL COMMENT '��ע',
         f_update_time  timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP COMMENT '����ʱ��',
         f_teaminfo blob DEFAULT NULL COMMENT '���ѷ�����Ϣ',       
         PRIMARY KEY ( f_user_id ),
		 INDEX f_user_id (f_user_id),
         KEY  f_id  ( f_id )
        ) ENGINE=InnoDB DEFAULT CHARSET=utf8

//alter table t_user add f_teaminfo blob default null comment "���ѷ�����Ϣ";
//alter table t_user drop column f_teaminfo;

//�û���ϵ��
//Ϊ�˱������࣬һ��Ҫ��֤f_user_id1С��f_user_id2
CREATE TABLE IF NOT EXISTS  t_user_relationship  (
         f_id  bigint(20) NOT NULL AUTO_INCREMENT COMMENT '����ID',
         f_user_id1  bigint(20) NOT NULL COMMENT '��һ���û�id',
         f_user_id2  bigint(20) NOT NULL COMMENT '�ڶ����û�id',		 
		 f_user1_teamname VARCHAR(32) NOT NULL DEFAULT "�ҵĺ���" COMMENT "�û�2���û�1�ĺ��ѷ�������",
		 f_user1_markname VARCHAR(32) COMMENT "�û�2���û�1�ı�ע����",		 
		 f_user2_teamname VARCHAR(32) NOT NULL DEFAULT "�ҵĺ���" COMMENT "�û�1���û�2�ĺ��ѷ�������",
		 f_user2_markname VARCHAR(32) COMMENT "�û�1���û�2�ı�ע����",
         f_update_time  timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP COMMENT '����ʱ��',
         f_remark  varchar(64) DEFAULT NULL COMMENT '��ע',
         PRIMARY KEY ( f_id ),
         KEY  f_id  ( f_id )
        ) ENGINE=InnoDB DEFAULT CHARSET=utf8

//ALTER TABLE t_user_relationship CHANGE f_create_time f_update_time timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP COMMENT '����ʱ��';		
//alter table t_user_relationship modify f_user1_teamname varchar(32) not null default "�ҵĺ���" comment "�û�2���û�1�ĺ��ѷ�������";
//alter table t_user_relationship modify f_user2_teamname varchar(32) not null default "�ҵĺ���" comment "�û�1���û�2�ĺ��ѷ�������";
//alter table t_user_relationship add f_user1_markname VARCHAR(32) COMMENT "�û�2���û�1�ı�ע����";
//alter table t_user_relationship add f_user2_markname VARCHAR(32) COMMENT "�û�1���û�2�ı�ע����";

//��Ϣ��¼��
CREATE TABLE IF NOT EXISTS  t_chatmsg  (
         f_id  bigint(20) NOT NULL AUTO_INCREMENT COMMENT '����ID',
         f_senderid  bigint(20) NOT NULL COMMENT '������id',
         f_targetid  bigint(20) NOT NULL COMMENT '������id',
		 f_msgcontent  BLOB NOT NULL COMMENT '��������',
         f_create_time  timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP COMMENT '����ʱ��',
         f_remark  varchar(64) DEFAULT NULL COMMENT '��ע',
         PRIMARY KEY ( f_id ),
         KEY  f_id  ( f_id )
        ) ENGINE=InnoDB DEFAULT CHARSET=utf8
